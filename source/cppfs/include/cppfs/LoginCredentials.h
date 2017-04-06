
#pragma once


#include <string>
#include <map>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    Login credentials for accessing file systems (e.g., username, password, key files, etc.)
*
*    This class is used to pass additional login credentials needed to acces file systems,
*    for example include username, password, port numbers, or paths to key files.
*    It can contain arbitrary key/value pairs that are defined and interpreted by
*    the respective file system implementations. See the documentation for a specific file
*    system implementation to learn what options it supports.
*/
class CPPFS_API LoginCredentials
{
public:
    /**
    *  @brief
    *    Constructor
    */
    LoginCredentials();

    /**
    *  @brief
    *    Copy constructor
    *
    *  @param[in] loginCredentials
    *    Right-hand value to copy
    */
    LoginCredentials(const LoginCredentials & loginCredentials);

    /**
    *  @brief
    *    Move constructor
    *
    *  @param[in] loginCredentials
    *    Right-hand value to copy
    */
    LoginCredentials(const LoginCredentials && loginCredentials);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~LoginCredentials();

    /**
    *  @brief
    *    Copy assignment operator
    *
    *  @param[in] loginCredentials
    *    Right-hand value to copy
    *
    *  @return
    *    Reference to this value
    */
    LoginCredentials & operator=(const LoginCredentials & loginCredentials);

    /**
    *  @brief
    *    Move assignment operator
    *
    *  @param[in] loginCredentials
    *    Right-hand value to move
    *
    *  @return
    *    Reference to this value
    */
    LoginCredentials & operator=(LoginCredentials && loginCredentials);

    /**
    *  @brief
    *    Load login credentials from file
    *
    *  @param[in] path
    *    Path to file
    *
    *  @return
    *    'true' on success, else 'false'
    */
    bool load(const std::string & path);

    /**
    *  @brief
    *    Save login credentials to file
    *
    *  @param[in] path
    *    Path to file
    *
    *  @return
    *    'true' on success, else 'false'
    */
    bool save(const std::string & path) const;

    /**
    *  @brief
    *    Check if a value is set for a given key
    *
    *  @param[in] key
    *    Key
    *
    *  @return
    *    'true' if a value for that key is set, else 'false'
    */
    bool isSet(const std::string & key) const;

    /**
    *  @brief
    *    Get value for key
    *
    *  @param[in] key
    *    Key
    *
    *  @return
    *    Value for key, "" if key does not exist
    */
    const std::string & value(const std::string & key) const;

    /**
    *  @brief
    *    Set value for key
    *
    *  @param[in] key
    *    Key
    *  @param[in] value
    *    Value for key
    */
    void setValue(const std::string & key, const std::string & value);

    /**
    *  @brief
    *    Set value for key
    *
    *  @param[in] key
    *    Key
    *  @param[in] value
    *    Value for key
    */
    void setValue(const std::string & key, std::string && value);


protected:
    std::map<std::string, std::string> m_values; ///< Key/value pairs
};


} // namespace cppfs
