
#pragma once


#include <memory>

#include <libssh2_sftp.h>

#include <cppfs/AbstractFileIteratorBackend.h>


namespace cppfs
{


class SshFileSystem;


/**
*  @brief
*    File iterator for a remove file system accessed by SSH
*/
class CPPFS_API SshFileIterator : public AbstractFileIteratorBackend
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
    SshFileIterator(std::shared_ptr<SshFileSystem> fs, const std::string & path);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~SshFileIterator();

    // Virtual AbstractFileIteratorBackend functions
    virtual std::unique_ptr<AbstractFileIteratorBackend> clone() const override;
    virtual AbstractFileSystem * fs() const override;
    virtual bool valid() const override;
    virtual std::string path() const override;
    virtual int index() const override;
    virtual std::string name() const override;
    virtual void next() override;


protected:
    void readNextEntry();


protected:
    std::shared_ptr<SshFileSystem>   m_fs;       ///< File system that created this iterator
    std::string                      m_path;     ///< Path to file or directory
    LIBSSH2_SFTP_HANDLE            * m_dir;      ///< Directory handle
    int                              m_index;    ///< Current entry index
    std::string                      m_filename; ///< Current entry file name
    LIBSSH2_SFTP_ATTRIBUTES          m_attrs;    ///< Current entry attributes
};


} // namespace cppfs
