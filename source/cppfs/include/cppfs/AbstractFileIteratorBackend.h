
#pragma once


#include <memory>
#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


class AbstractFileSystem;


/**
*  @brief
*    Interface for iterating on directories
*/
class CPPFS_API AbstractFileIteratorBackend
{
public:
    /**
    *  @brief
    *    Constructor
    */
    AbstractFileIteratorBackend();

    /**
    *  @brief
    *    Destructor
    */
    virtual ~AbstractFileIteratorBackend();

    /**
    *  @brief
    *    Create a copy of this iterator
    *
    *  @return
    *    File iterator
    */
    virtual std::unique_ptr<AbstractFileIteratorBackend> clone() const = 0;

    /**
    *  @brief
    *    Get file system
    *
    *  @return
    *    File system (must NOT be null)
    */
    virtual AbstractFileSystem * fs() const = 0;

    /**
    *  @brief
    *    Check if iterator points to a valid item
    *
    *  @return
    *    'true' if valid, else 'false'
    */
    virtual bool valid() const = 0;

    /**
    *  @brief
    *    Get path in file system on which the iterator operates
    *
    *  @return
    *    Path
    */
    virtual std::string path() const = 0;

    /**
    *  @brief
    *    Get current index of iterator in the directory
    *
    *  @return
    *    Index, -1 if invalid
    */
    virtual int index() const = 0;

    /**
    *  @brief
    *    Get name of current directory item
    *
    *  @return
    *    File name
    */
    virtual std::string name() const = 0;

    /**
    *  @brief
    *    Advance to the next item
    */
    virtual void next() = 0;
};


} // namespace cppfs
