
#include <cppfs/windows/LocalFileSystem.h>

#include <cppfs/FileHandle.h>
#include <cppfs/windows/LocalFileHandle.h>
#include <cppfs/windows/LocalFileWatcher.h>


namespace cppfs
{


LocalFileSystem::LocalFileSystem()
{
}

LocalFileSystem::~LocalFileSystem()
{
}

FileHandle LocalFileSystem::open(const std::string & path)
{
    return open(std::string(path));
}

FileHandle LocalFileSystem::open(std::string && path)
{
    return FileHandle(
        std::unique_ptr<AbstractFileHandleBackend>(
            new LocalFileHandle(shared_from_this(), path)
        )
    );
}

std::unique_ptr<AbstractFileWatcherBackend> LocalFileSystem::createFileWatcher(FileWatcher & fileWatcher)
{
    return std::unique_ptr<AbstractFileWatcherBackend>(
            new LocalFileWatcher(&fileWatcher, shared_from_this())
    );
}


} // namespace cppfs
