
#include <cppfs/FileWatcher.h>

#include <algorithm>

#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FileEventHandler.h>
#include <cppfs/AbstractFileSystem.h>
#include <cppfs/null/NullFileWatcher.h>


namespace cppfs
{


FileWatcher::FileWatcher()
: m_backend(fs::localFS()->createFileWatcher(*this))
{
}

FileWatcher::FileWatcher(AbstractFileSystem * fs)
: m_backend(fs ? fs->createFileWatcher(*this) : nullptr)
{
}

FileWatcher::FileWatcher(FileWatcher && fileWatcher)
: m_backend(std::move(fileWatcher.m_backend))
{
}

FileWatcher::~FileWatcher()
{
}

FileWatcher & FileWatcher::operator=(FileWatcher && fileWatcher)
{
    m_backend = std::move(fileWatcher.m_backend);

    return *this;
}

AbstractFileSystem * FileWatcher::fs() const
{
    return m_backend ? m_backend->fs() : nullptr;
}

void FileWatcher::add(const FileHandle & fileHandle, unsigned int mode)
{
    // Check backend
    if (!m_backend) {
        return;
    }

    // Check that file handle belongs to the same file system as the watcher
    if (fileHandle.fs() != fs()) {
        return;
    }

    // Add file to watcher
    m_backend->add(fileHandle.path(), mode);
}

void FileWatcher::addHandler(FileEventHandler * eventHandler)
{
    // Check that event handler is valid and not already registered
    if (!eventHandler || std::find(m_eventHandlers.begin(), m_eventHandlers.end(), eventHandler) != m_eventHandlers.end()) {
        return;
    }

    // Add event handler to list
    m_eventHandlers.push_back(eventHandler);
}

void FileWatcher::addHandler(EventFunc funcFileEvent)
{
    // Create event handler
    auto ptr = std::unique_ptr<FunctionalFileEventHandler>(new FunctionalFileEventHandler(funcFileEvent));

    // Register event handler
    addHandler(ptr.get());

    // Take ownership
    m_ownEventHandlers.push_back(std::move(ptr));
}

void FileWatcher::removeHandler(FileEventHandler * eventHandler)
{
    // Check if event handler is registered
    auto it = std::find(m_eventHandlers.begin(), m_eventHandlers.end(), eventHandler);
    if (it != m_eventHandlers.end()) {
        // Remove event handler
        m_eventHandlers.erase(it);
    }
}

void FileWatcher::watch()
{
    // Check backend
    if (!m_backend) {
        return;
    }

    // Watch files
    m_backend->watch();
}

void FileWatcher::onFileEvent(const std::string & path, FileEvent event)
{
    // [TODO] Pass FileHandle instead of string
    // [TODO] Make sure that path is fully qualified

    // Open file
    FileHandle fh = fs::open(path);

    // Call file event handlers
    for (auto * eventHandler : m_eventHandlers) {
        eventHandler->onFileEvent(fh, event);
    }
}


} // name cppfs
