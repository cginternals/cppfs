
#pragma once


#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    Access system information
*/
namespace system
{


/**
*  @brief
*    Get home directory of the current user
*
*  @return
*    Home directory (native path)
*/
CPPFS_API std::string homeDir();

/**
*  @brief
*    Get config directory for the named application
*
*  @param[in] application
*    Application name
*
*  @return
*    Config directory (native path)
*/
CPPFS_API std::string configDir(const std::string & application);


} // namespace system


} // namespace cppfs
