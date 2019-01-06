
#include <cppfs/posix/LocalFileSystem.h>

#include <cppfs/FileHandle.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/AbstractFileWatcherBackend.h>
#include <cppfs/posix/LocalFileHandle.h>

#ifdef SYSTEM_LINUX
    #include <cppfs/linux/LocalFileWatcher.h>
#endif


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
#ifdef SYSTEM_LINUX
    return std::unique_ptr<AbstractFileWatcherBackend>(
            new LocalFileWatcher(&fileWatcher, shared_from_this())
    );
#else
    return nullptr;
#endif
}


} // namespace cppfs
