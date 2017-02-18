
#pragma once


#include <memory>

#include <cppfs/AbstractFileIteratorBackend.h>


namespace cppfs
{


class WinFileSystem;


/**
*  @brief
*    File iterator for the local file system
*/
class CPPFS_API WinFileIterator : public AbstractFileIteratorBackend
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fs
    *    File system that created this iterator
    *  @param[in] path
    *    Path to file or directory
    */
    WinFileIterator(std::shared_ptr<WinFileSystem> fs, const std::string & path);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~WinFileIterator();

    // Virtual AbstractFileIteratorBackend functions
    virtual AbstractFileIteratorBackend * clone() const override;
    virtual AbstractFileSystem * fs() const override;
    virtual bool valid() const override;
    virtual std::string path() const override;
    virtual int index() const override;
    virtual std::string name() const override;
    virtual void next() override;


protected:
    void readNextEntry();


protected:
    std::shared_ptr<WinFileSystem>   m_fs;         ///< File system that created this iterator
    std::string                      m_path;       ///< Path to file or directory
    int                              m_index;      ///< Index of the current entry
	void                           * m_findHandle; ///< Search handle
	void                           * m_findData;   ///< Information about the current file
};


} // namespace cppfs
