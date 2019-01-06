
#include <cppfs/linux/LocalFileWatcher.h>

#include <unistd.h>
#include <limits.h>
#include <sys/inotify.h>

#include <cppfs/cppfs.h>
#include <cppfs/FilePath.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/posix/LocalFileSystem.h>
#include <cppfs/posix/LocalFileIterator.h>


namespace cppfs
{


LocalFileWatcher::LocalFileWatcher(FileWatcher * fileWatcher, std::shared_ptr<LocalFileSystem> fs)
: AbstractFileWatcherBackend(fileWatcher)
, m_fs(fs)
, m_inotify(-1)
{
    // Create inotify instance
    m_inotify = inotify_init();
}

LocalFileWatcher::~LocalFileWatcher()
{
    // Close watch handles
    for (auto it : m_watchers) {
        inotify_rm_watch(m_inotify, it.first);
    }

    // Close inotify instance
    close(m_inotify);
}

AbstractFileSystem * LocalFileWatcher::fs() const
{
    // Return file system
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void LocalFileWatcher::add(FileHandle & dir, unsigned int events, RecursiveMode recursive)
{
    // Get watch mode
    uint32_t flags = 0;
    if (events & FileCreated)     flags |= IN_CREATE;
    if (events & FileRemoved)     flags |= IN_DELETE;
    if (events & FileModified)    flags |= IN_MODIFY;
    if (events & FileAttrChanged) flags |= IN_ATTRIB;


    // Create watcher
    int handle = inotify_add_watch(m_inotify, dir.path().c_str(), flags);
    if (handle < 0) {
        return;
    }

    // Watch directories recursively
    if (recursive == Recursive) {
        // List directory entries
        for (auto it = dir.begin(); it != dir.end(); ++it)
        {
            // Check if entry is a directory
            FileHandle fh2 = dir.open(*it);
            if (fh2.isDirectory()) {
                // Watch directory
                add(fh2, events, recursive);
            }
        }
    }

    // Associate watcher handle with file handle
    m_watchers[handle].dir       = dir;
    m_watchers[handle].events    = events;
    m_watchers[handle].recursive = recursive;
}

void LocalFileWatcher::watch(int timeout)
{
    // Create buffer for receiving events
    size_t bufSize = 64 * (sizeof(inotify_event) + NAME_MAX);
    std::vector<char> buffer;
    buffer.resize(bufSize);

    // Set timeout
    if (timeout >= 0) {
        // Create file descriptor set
        fd_set set;
        FD_ZERO(&set);
        FD_SET(m_inotify, &set);

        // Convert timeout into timeval struct
        struct timeval tval;
        tval.tv_sec = timeout / 1000;
        tval.tv_usec = (timeout - tval.tv_sec * 1000) * 1000;

        // Set timeout on file descriptor
        int rv = select(m_inotify + 1, &set, NULL, NULL, &tval);
        if (rv <= 0) {
            return;
        }
    }

    // Read events
    int size = read(m_inotify, buffer.data(), bufSize);
    if (size < 0) {
        return;
    }

    // Process all events
    int i = 0;
    while (i < size) {
        // Get event
        auto * event = reinterpret_cast<inotify_event *>(&buffer.data()[i]);
        if (event->len) {
            // Get event
            FileEvent eventType = (FileEvent)0;
                 if (event->mask & IN_CREATE) eventType = FileCreated;
            else if (event->mask & IN_DELETE) eventType = FileRemoved;
            else if (event->mask & IN_MODIFY) eventType = FileModified;
            else if (event->mask & IN_ATTRIB) eventType = FileAttrChanged;

            // Get watcher
            auto & watcher = m_watchers[event->wd];

            // Get file handle
            FileHandle fh = (event->len > 0 ? watcher.dir.open(std::string(event->name)) : watcher.dir);

            // Watch new directories
            if (fh.isDirectory() && eventType == FileCreated && watcher.recursive == Recursive) {
                add(fh, watcher.events, watcher.recursive);
            }

            // Invoke callback function
            onFileEvent(fh, eventType);
        }

        // Next event
        i += sizeof(inotify_event) + event->len;
    }
}


} // namespace cppfs
