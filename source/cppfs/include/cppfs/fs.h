
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
*    Read entire file to string
*
*  @param[in] path
*    Path to file
*
*  @return
*    File content, "" on error
*/
CPPFS_API std::string readFile(const std::string & path);


} // namespace fs


} // namespace cppfs
