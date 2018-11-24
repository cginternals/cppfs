
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

std::unique_ptr<AbstractFileWatcherBackend> NullFileWatcher::clone() const
{
    return std::unique_ptr<AbstractFileWatcherBackend>(new NullFileWatcher(m_fileWatcher, m_fs));
}

AbstractFileSystem * NullFileWatcher::fs() const
{
    return m_fs.get();
}

void NullFileWatcher::add(const std::string &, unsigned int)
{
}

void NullFileWatcher::watch()
{
}


} // namespace cppfs