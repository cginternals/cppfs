
#pragma once


#include <memory>
#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


class AbstractFileIteratorBackend;


/**
*  @brief
*    File iterator
*/
class CPPFS_API FileIterator
{
public:
    /**
    *  @brief
    *    Constructor
    */
    FileIterator();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] backend
    *    Concrete file handle backend
    */
    FileIterator(std::unique_ptr<AbstractFileIteratorBackend> && backend);

    /**
    *  @brief
    *    Copy constructor
    *
    *  @param[in] fileHandle
    *    Source handle
    */
    FileIterator(const FileIterator & fileIterator);

    /**
    *  @brief
    *    Move constructor
    *
    *  @param[in] fileHandle
    *    Source handle
    */
    FileIterator(FileIterator && fileIterator);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FileIterator();

    /**
    *  @brief
    *    Copy operator
    *
    *  @param[in] fileIterator
    *    Source handle
    */
    FileIterator & operator=(const FileIterator & fileIterator);

    /**
    *  @brief
    *    Get name of current directory item
    *
    *  @return
    *    File name
    */
    std::string operator*() const;

    /**
    *  @brief
    *    Advance to the next item
    */
    void operator++();

    /**
    *  @brief
    *    Compare two iterators
    *
    *  @param[in] it
    *    Iterator
    *
    *  @return
    *    'true' if iterators are equal, else 'false'
    */
    bool operator==(const FileIterator & it) const;

    /**
    *  @brief
    *    Compare two iterators
    *
    *  @param[in] it
    *    Iterator
    *
    *  @return
    *    'true' if iterators are not equal, else 'false'
    */
    bool operator!=(const FileIterator & it) const;


protected:
    std::unique_ptr<AbstractFileIteratorBackend> m_backend;
};


} // namespace cppfs
