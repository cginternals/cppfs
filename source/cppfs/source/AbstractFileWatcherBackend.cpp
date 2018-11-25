
#include <cppfs/AbstractFileWatcherBackend.h>

#include <cppfs/FileWatcher.h>


namespace cppfs
{


AbstractFileWatcherBackend::AbstractFileWatcherBackend(FileWatcher & fileWatcher)
: m_fileWatcher(fileWatcher)
{
}

AbstractFileWatcherBackend::~AbstractFileWatcherBackend()
{
}

void AbstractFileWatcherBackend::onFileEvent(FileHandle & fileHandle, FileEvent event)
{
    m_fileWatcher.onFileEvent(fileHandle, event);
}


} // namespace cppfs
