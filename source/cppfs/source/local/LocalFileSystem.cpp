
#include <cppfs/local/LocalFileSystem.h>

#include <cppfs/FileHandle.h>
#include <cppfs/local/LocalFileHandle.h>


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
    return FileHandle(new LocalFileHandle(shared_from_this(), path));
}


} // namespace cppfs
