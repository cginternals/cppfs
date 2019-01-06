
#pragma once


#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    File permission flags
*/
enum FilePermissions
{
    UserRead   = 0x400,
    UserWrite  = 0x200,
    UserExec   = 0x100,
    GroupRead  = 0x040,
    GroupWrite = 0x020,
    GroupExec  = 0x010,
    OtherRead  = 0x004,
    OtherWrite = 0x002,
    OtherExec  = 0x001,
    SetUid     = 04000,
    SetGid     = 02000,
    Sticky     = 01000
};

/**
*  @brief
*    Type of event on the file system
*/
enum FileEvent
{
    FileCreated     = 0x01, ///< A file or directory has been created
    FileRemoved     = 0x02, ///< A file or directory has been removed
    FileModified    = 0x04, ///< A file or directory has been modified
    FileAttrChanged = 0x08  ///< Attributes on a file or directory have been modified
};

/**
*  @brief
*    Recursive mode for operation that can run recursively or non-recursively
*/
enum RecursiveMode
{
    NonRecursive = 0, ///< Run recursively
    Recursive         ///< Run non-recursively
};


} // namespace cppfs
