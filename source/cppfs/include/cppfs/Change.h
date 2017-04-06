
#pragma once


#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    Representation of one change to a directory tree
*/
class CPPFS_API Change
{
public:
    /**
    *  @brief
    *    Type of operation on the file system
    */
    enum Operation
    {
        None = 0,   ///< No operation
        CopyFile,   ///< A file has been added or modified
        CopyDir,    ///< A directory tree has been added
        RemoveFile, ///< A file has been removed
        RemoveDir   ///< A directory tree has been removed
    };


public:
    /**
    *  @brief
    *    Constructor
    */
    Change();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] operation
    *    Operation type
    *  @param[in] path
    *    Path on which the operation takes place
    */
    Change(Operation operation, const std::string & path);

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] operation
    *    Operation type
    *  @param[in] path
    *    Path on which the operation takes place
    */
    Change(Operation operation, std::string && path);

    /**
    *  @brief
    *    Copy constructor
    *
    *  @param[in] change
    *    Other change
    */
    Change(const Change & change);

    /**
    *  @brief
    *    Move constructor
    *
    *  @param[in] change
    *    Other change
    */
    Change(Change && change);

    /**
    *  @brief
    *    Destructor
    */
    ~Change();

    /**
    *  @brief
    *    Copy operator
    *
    *  @param[in] change
    *    Other change
    */
    Change & operator=(const Change & change);

    /**
    *  @brief
    *    Move operator
    *
    *  @param[in] change
    *    Other change
    */
    Change & operator=(Change && change);

    /**
    *  @brief
    *    Get string representation of the change
    *
    *  @return
    *    String representation of the change
    */
    std::string toString() const;

    /**
    *  @brief
    *    Get operation
    *
    *  @return
    *    Operation type
    */
    Operation operation() const;

    /**
    *  @brief
    *    Get path
    *
    *  @return
    *    Path on which the operation takes place
    */
    const std::string & path() const;


protected:
    Operation   m_operation; ///< Operation type
    std::string m_path;      ///< Path on which the operation takes place
};


} // namespace cppfs
