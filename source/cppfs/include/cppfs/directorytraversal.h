
#pragma once


#include <string>
#include <vector>
#include <functional>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    List all files in a directory
*
*  @param[in] directory
*    Path to directory (exluding a trailing '`/`'!)
*  @param[in] recursive
*    Search recursively in sub-directories?
*  @param[out] files
*    List of files
*
*  @remark
*    Lists all files in the directory, including all
*    files in sub-directories if recursive is true.
*
*  @remark
*    Only files are listed, directories are not included.
*
*  @remark
*    The search path is included in the file name, e.g.,
*    getFile("dir") may result in ["dir/file1.txt", "dir/file2.png", ...].
*/
CPPFS_API void getFiles(const std::string & directory, bool recursive, std::vector<std::string> & files);

/**
*  @brief
*    List all files in a directory
*
*  @param[in] directory
*    Path to directory (exluding a trailing '`/`'!)
*  @param[in] recursive
*    Search recursively in sub-directories?
*
*  @return
*    List of files
*/
CPPFS_API std::vector<std::string> getFiles(const std::string & directory, bool recursive);

/**
*  @brief
*    List all files in a directory
*
*  @param[in] directories
*    Vector of paths to directories (exluding a trailing '`/`'!)
*  @param[in] recursive
*    Search recursively in sub-directories?
*
*  @return
*    List of files
*/
CPPFS_API std::vector<std::string> getFiles(const std::vector<std::string> & directories, bool recursive);

/**
*  @brief
*    Scan directory for files with a specific filename extension
*
*  @param[in] directory
*    Path to directory
*  @param[in] fileExtension
*    File extension ('`*`' for all files)
*  @param[in] recursive
*    Search recursively in sub-directories?
*
*  @return
*    List of found files, including the directory name
*/
CPPFS_API std::vector<std::string> scanDirectory(const std::string & directory, const std::string & fileExtension, bool recursive = false);

/**
*  @brief
*    Scan directory for files with a specific filename extension
*
*  @param[in] directory
*    Path to directory
*  @param[in] fileExtension
*    File extension ('`*`' for all files)
*  @param[in] recursive
*    Search recursively in sub-directories?
*  @param[in] callback
*    Function that is called for each found file
*/
CPPFS_API void scanDirectory(const std::string & directory, const std::string & fileExtension, bool recursive, const std::function<void(const std::string &)> & callback);


} // namespace cppfs
