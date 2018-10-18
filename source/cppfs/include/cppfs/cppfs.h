
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


} // namespace cppfs
