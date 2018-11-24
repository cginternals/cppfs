
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

void AbstractFileWatcherBackend::onFileEvent(const std::string & path, FileEvent event)
{
    m_fileWatcher.onFileEvent(path, event);
}


} // namespace cppfs
