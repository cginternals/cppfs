
#include <cppfs/windows/WinFileSystem.h>

#include <cppfs/FileHandle.h>
#include <cppfs/windows/WinFileHandle.h>


namespace cppfs
{


WinFileSystem::WinFileSystem()
{
}

WinFileSystem::~WinFileSystem()
{
}

FileHandle WinFileSystem::open(const std::string & path)
{
    return FileHandle(new WinFileHandle(shared_from_this(), path));
}


} // namespace cppfs
