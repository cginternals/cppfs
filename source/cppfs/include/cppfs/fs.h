
#pragma once


#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


class FileHandle;
class LoginCredentials;


/**
*  @brief
*    Global file system functions
*/
namespace fs
{


/**
*  @brief
*    Open a file or directory
*
*  @param[in] path
*    Path to file or directory
*  @param[in] credentials
*    Optional login credentials (can be null)
*
*  @return
*    File handle
*/
CPPFS_API FileHandle open(const std::string & path, const LoginCredentials * credentials = nullptr);

/**
*  @brief
*    Copy directory recursively
*
*  @param[in] srcDir
*    Source directory
*  @param[in] dstDir
*    Destination directory
*
*  @remarks
*    Destination directory points to the actual directory that
*    is to be created, not its parent!
*
*    Example:
*      fs::copyDirectory(fs::open("/projects/project1"), fs::open("/backup/projects/project1"))
*/
CPPFS_API void copyDirectory(const FileHandle & srcDir, FileHandle & dstDir);

/**
*  @brief
*    Remove directory recursively
*
*  @param[in] dir
*    Directory handle
*/
CPPFS_API void removeDirectory(FileHandle & dir);

/**
*  @brief
*    Compute sha1 hash for string
*
*  @param[in] str
*    String
*
*  @return
*    SHA1 hash
*/
CPPFS_API std::string sha1(const std::string & str);

/**
*  @brief
*    Compute sha1 hash for file
*
*  @param[in] file
*    File handle
*
*  @return
*    SHA1 hash, "" on error
*/
CPPFS_API std::string sha1(const FileHandle & file);


} // namespace fs


} // namespace cppfs
