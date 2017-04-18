
#pragma once


#include <memory>

#include <cppfs/AbstractFileHandleBackend.h>


namespace cppfs
{


class SshFileSystem;


/**
*  @brief
*    File handle for a remote file system accessed by SSH
*/
class CPPFS_API SshFileHandle : public AbstractFileHandleBackend
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fs
    *    File system that created this handle
    *  @param[in] path
    *    Path to file or directory
    */
    SshFileHandle(std::shared_ptr<SshFileSystem> fs, const std::string & path);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~SshFileHandle();

    // Virtual AbstractFileHandleBackend functions
    virtual std::unique_ptr<AbstractFileHandleBackend> clone() const override;
    virtual AbstractFileSystem * fs() const override;
    virtual void updateFileInfo() override;
    virtual std::string path() const override;
    virtual bool exists() const override;
    virtual bool isFile() const override;
    virtual bool isDirectory() const override;
    virtual bool isSymbolicLink() const override;
    virtual std::vector<std::string> listFiles() const override;
    virtual std::unique_ptr<AbstractFileIteratorBackend> begin() const override;
    virtual unsigned int size() const override;
    virtual unsigned int accessTime() const override;
    virtual unsigned int modificationTime() const override;
    virtual unsigned int userId() const override;
    virtual void setUserId(unsigned int uid) override;
    virtual unsigned int groupId() const override;
    virtual void setGroupId(unsigned int gid) override;
    virtual unsigned long permissions() const override;
    virtual void setPermissions(unsigned long permissions) override;
    virtual bool createDirectory() override;
    virtual bool removeDirectory() override;
    virtual bool copy(AbstractFileHandleBackend & dest) override;
    virtual bool move(AbstractFileHandleBackend & dest) override;
    virtual bool createLink(AbstractFileHandleBackend & dest) override;
    virtual bool createSymbolicLink(AbstractFileHandleBackend & dest) override;
    virtual bool rename(const std::string & filename) override;
    virtual bool remove() override;
    virtual std::unique_ptr<std::istream> createInputStream(std::ios_base::openmode mode) const override;
    virtual std::unique_ptr<std::ostream> createOutputStream(std::ios_base::openmode mode) override;


protected:
    void readFileInfo() const;
    void readLinkInfo() const;


protected:
    std::shared_ptr<SshFileSystem> m_fs;       ///< File system that created this handle
    std::string                    m_path;     ///< Path to file or directory
    mutable void                 * m_fileInfo; ///< Information about the current file (resolves links, created on demand)
    mutable void                 * m_linkInfo; ///< Information about the current file (does not resolve links, created on demand)
};


} // namespace cppfs
