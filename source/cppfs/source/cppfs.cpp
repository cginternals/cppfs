
#include <iostream>

#include <cppfs/cppfs-version.h>
#include <cppfs/cppfs.h>


namespace cppfs
{


void printInfo()
{
    std::cout << "cppfs " << std::string(CPPFS_VERSION) << std::endl;
    std::cout << std::endl;
}


} // namespace cppfs
