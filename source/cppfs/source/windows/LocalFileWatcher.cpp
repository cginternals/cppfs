
#include <cppfs/windows/LocalFileWatcher.h>

#include <cppfs/windows/LocalFileSystem.h>


namespace cppfs
{


LocalFileWatcher::LocalFileWatcher(FileWatcher & fileWatcher, std::shared_ptr<LocalFileSystem> fs)
: AbstractFileWatcherBackend(fileWatcher)
, m_fs(fs)
{
}

LocalFileWatcher::~LocalFileWatcher()
{
}

AbstractFileSystem * LocalFileWatcher::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void LocalFileWatcher::add(const FileHandle &, unsigned int, RecursiveMode)
{
    // [TODO] Implement for Windows
}

void LocalFileWatcher::watch()
{
    // [TODO] Implement for Windows
}


} // namespace cppfs
