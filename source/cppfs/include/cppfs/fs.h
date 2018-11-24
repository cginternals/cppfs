
#pragma once


#include <memory>
#include <string>

#include <cppfs/AbstractFileSystem.h>


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
*    Get local file system
*
*  @return
*    Pointer to local file system (never null)
*/
CPPFS_API std::shared_ptr<AbstractFileSystem> localFS();

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
*    Get base64 encoding for string
*
*  @param[in] str
*    String
*
*  @return
*    Base64 encoded string
*/
CPPFS_API std::string base64(const std::string & str);

/**
*  @brief
*    Get decoded string from base64 encoding
*
*  @param[in] base64
*    Base64 encoded string
*
*  @return
*    Decoded string
*/
CPPFS_API std::string fromBase64(const std::string & base64);

/**
*  @brief
*    Convert hash buffer into string
*
*  @param[in] hash
*    Hash buffer
*
*  @return
*    Hash string
*/
CPPFS_API std::string hashToString(const unsigned char * hash);


} // namespace fs


} // namespace cppfs
