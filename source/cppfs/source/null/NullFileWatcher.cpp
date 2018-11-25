
#include <cppfs/null/NullFileWatcher.h>


namespace cppfs
{


NullFileWatcher::NullFileWatcher(FileWatcher & fileWatcher)
: AbstractFileWatcherBackend(fileWatcher)
{
}

NullFileWatcher::NullFileWatcher(FileWatcher & fileWatcher, std::shared_ptr<AbstractFileSystem> fs)
: AbstractFileWatcherBackend(fileWatcher)
, m_fs(fs)
{
}

NullFileWatcher::~NullFileWatcher()
{
}

AbstractFileSystem * NullFileWatcher::fs() const
{
    return m_fs.get();
}

void NullFileWatcher::add(FileHandle &, unsigned int, RecursiveMode)
{
}

void NullFileWatcher::watch()
{
}


} // namespace cppfs
