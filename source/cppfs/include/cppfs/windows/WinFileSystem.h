
#pragma once


#include <memory>

#include <cppfs/AbstractFileSystem.h>


namespace cppfs
{


/**
*  @brief
*    Representation of the local file system
*/
class CPPFS_API WinFileSystem : public AbstractFileSystem, public std::enable_shared_from_this<WinFileSystem>
{
public:
    /**
    *  @brief
    *    Constructor
    */
    WinFileSystem();

    /**
    *  @brief
    *    Destructor
    */
    virtual ~WinFileSystem();

    // Virtual AbstractFileSystem functions
    virtual FileHandle open(const std::string & path) override;
};


} // namespace cppfs
