
#include <cppfs/posix/LocalFileWatcher.h>

#include <unistd.h>
#include <limits.h>
#include <sys/inotify.h>

#include <cppfs/cppfs.h>
#include <cppfs/FilePath.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/posix/LocalFileSystem.h>
#include <cppfs/posix/LocalFileIterator.h>


namespace cppfs
{


LocalFileWatcher::LocalFileWatcher(FileWatcher & fileWatcher, std::shared_ptr<LocalFileSystem> fs)
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
    for (int handle : m_watchers) {
        inotify_rm_watch(m_inotify, handle);
    }

    // Close inotify instance
    close(m_inotify);
}

std::unique_ptr<AbstractFileWatcherBackend> LocalFileWatcher::clone() const
{
    // Clone watcher
    return std::unique_ptr<AbstractFileWatcherBackend>(new LocalFileWatcher(m_fileWatcher, m_fs));
}

AbstractFileSystem * LocalFileWatcher::fs() const
{
    // Return file system
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void LocalFileWatcher::add(const std::string & path, unsigned int mode)
{
    // Get watch mode
    uint32_t flags = 0;
    if (mode & FileCreated)  flags |= IN_CREATE;
    if (mode & FileRemoved)  flags |= IN_DELETE;
    if (mode & FileModified) flags |= IN_MODIFY;

    // Create watcher
    int handle = inotify_add_watch(m_inotify, path.c_str(), flags);
    if (handle < 0) {
        return;
    }

    // Add watcher handle
    m_watchers.push_back(handle);
}

void LocalFileWatcher::watch()
{
    // Create buffer for receiving events
    size_t bufSize = 64 * (sizeof(inotify_event) + NAME_MAX);
    std::vector<char> buffer;
    buffer.resize(bufSize);

    // Read events
    int numEvents = read(m_inotify, buffer.data(), bufSize);
    if (numEvents < 0) {
        return;
    }

    // Process all events
    for (int i=0; i<numEvents; i++) {
        // Get event
        auto * event = reinterpret_cast<inotify_event *>(&buffer.data()[i]);
        if (event->len) {
            // Get path
            std::string path = event->name;

            // Get event
            FileEvent eventType = (FileEvent)0;
                 if (event->mask & IN_CREATE) eventType = FileCreated;
            else if (event->mask & IN_DELETE) eventType = FileRemoved;
            else if (event->mask & IN_MODIFY) eventType = FileModified;

            // Invoke callback function
            onFileEvent(path, eventType);
        }

        // Next event
        i += sizeof(inotify_event) + event->len;
    }
}


} // namespace cppfs