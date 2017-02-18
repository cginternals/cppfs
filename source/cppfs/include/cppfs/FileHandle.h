
#pragma once


#include <memory>
#include <vector>
#include <string>
#include <iostream>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


class FileIterator;
class AbstractFileHandleBackend;


/**
*  @brief
*    Handle for a file or directory
*
*  @remarks
*    A file handle is used to access a file or directory.
*    It can be obtained either by calling open() on a
*    FileSystem object, or by iterating over directories
*    using the FileSystemIterator.
*
*    If the handle points to a valid file or directory,
*    exists() returns true, otherwise false. The type of
*    the entry can be determined by the methods isFile()
*    and isDirectory().
*
*    File handles can be copied or moved. Overhead for these
*    functions are limited, so for example, file or directories
*    will not be opened automatically.
*/
class CPPFS_API FileHandle
{
public:
    /**
    *  @brief
    *    Constructor
    */
    FileHandle();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] backend
    *    Concrete file handle backend
    */
    FileHandle(AbstractFileHandleBackend * backend);

    /**
    *  @brief
    *    Copy constructor
    *
    *  @param[in] fileHandle
    *    Source handle
    */
    FileHandle(const FileHandle & fileHandle);

    /**
    *  @brief
    *    Move constructor
    *
    *  @param[in] fileHandle
    *    Source handle
    */
    FileHandle(FileHandle && fileHandle);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FileHandle();

    /**
    *  @brief
    *    Copy operator
    *
    *  @param[in] fileHandle
    *    Source handle
    */
    FileHandle & operator=(const FileHandle & fileHandle);

    /**
    *  @brief
    *    Get path
    *
    *  @return
    *    Path to file or directory
    */
    std::string path() const;

    /**
    *  @brief
    *    Get filename
    *
    *  @return
    *    Filename
    */
    std::string fileName() const;

    /**
    *  @brief
    *    Update file information
    *
    *  @remarks
    *    Must reload and update the file information. It will
    *    for example be called after a file has been created,
    *    copied, or removed to ensure that the file information
    *    returned by the handle is correct.
    */
    void updateFileInfo();

    /**
    *  @brief
    *    Check if file or directory exists
    *
    *  @return
    *    'true' if it exists, else 'false'
    */
    bool exists() const;

    /**
    *  @brief
    *    Check if item is a file
    *
    *  @return
    *    'true' if it is a file, else 'false'
    */
    bool isFile() const;

    /**
    *  @brief
    *    Check if item is a directory
    *
    *  @return
    *    'true' if it is a directory, else 'false'
    */
    bool isDirectory() const;

    /**
    *  @brief
    *    List files in directory
    *
    *  @return
    *    List of files, empty list if this is not a valid directory
    */
    std::vector<std::string> listFiles() const;

    /**
    *  @brief
    *    Get iterator that points to the first directory entry
    *
    *  @return
    *    Iterator
    */
    FileIterator begin() const;

    /**
    *  @brief
    *    Get iterator that points behind the last directory entry
    *
    *  @return
    *    Iterator
    */
    FileIterator end() const;

    /**
    *  @brief
    *    Get file size
    *
    *  @return
    *    Size if handle points to a file, else 0
    */
    unsigned int size() const;

    /**
    *  @brief
    *    Get time of last access
    *
    *  @return
    *    Time stamp
    */
    unsigned int accessTime() const;

    /**
    *  @brief
    *    Get time of last modification
    *
    *  @return
    *    Time stamp
    */
    unsigned int modificationTime() const;

    /**
    *  @brief
    *    Get ID of owning user
    *
    *  @return
    *    User ID
    */
    unsigned int userId() const;

    /**
    *  @brief
    *    Set owning user
    *
    *  @param[in] uid
    *    User ID
    */
    void setUserId(unsigned int uid);

    /**
    *  @brief
    *    Get ID of owning group
    *
    *  @return
    *    Group ID
    */
    unsigned int groupId() const;

    /**
    *  @brief
    *    Set owning group
    *
    *  @param[in] gid
    *    Group ID
    */
    void setGroupId(unsigned int gid);

    /**
    *  @brief
    *    Get file permissions
    *
    *  @return
    *    File permissions
    */
    unsigned long permissions() const;

    /**
    *  @brief
    *    Set file permissions
    *
    *  @param[in] permissions
    *    File permissions
    */
    void setPermissions(unsigned long permissions);

    /**
    *  @brief
    *    Get handle to the parent directory
    *
    *  @return
    *    File handle
    */
    FileHandle parentDirectory() const;

    /**
    *  @brief
    *    Open file using a relative path from the current file/directory
    *
    *  @param[in] path
    *    Relative path
    *
    *  @return
    *    File handle
    */
    FileHandle open(const std::string & path);

    /**
    *  @brief
    *    Create directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool makeDirectory();

    /**
    *  @brief
    *    Remove directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool removeDirectory();

    /**
    *  @brief
    *    Copy file
    *
    *  @param[in] dest
    *    Destination file or directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool copy(FileHandle & dest);

    /**
    *  @brief
    *    Move file
    *
    *  @param[in] dest
    *    Destination file or directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool move(FileHandle & dest);

    /**
    *  @brief
    *    Rename file or directory
    *
    *  @param[in] filename
    *    File name
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool rename(const std::string & filename);

    /**
    *  @brief
    *    Remove file
    *
    *  @return
    *    'true' if successful, else 'false'
    *
    *  @remarks
    *    Does only work if the handle points to a valid file,
    *    not a directory.
    */
    bool remove();

    /**
    *  @brief
    *    Create input stream to read from the file
    *
    *  @param[in] mode
    *    Opening mode flags
    *
    *  @return
    *    Input stream, null on error
    *
    *  @remarks
    *    The created stream object has to be destroyed be the caller.
    */
    std::istream * createInputStream(std::ios_base::openmode mode = std::ios_base::in) const;

    /**
    *  @brief
    *    Create output stream to write to the file
    *
    *  @param[in] mode
    *    Opening mode flags
    *
    *  @return
    *    Output stream, null on error
    *
    *  @remarks
    *    The created stream object has to be destroyed be the caller.
    */
    std::ostream * createOutputStream(std::ios_base::openmode mode = std::ios_base::out);


protected:
    /**
    *  @brief
    *    Copy file by stream copy
    *
    *  @param[in] dest
    *    Destination file or directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool genericCopy(FileHandle & dest);

    /**
    *  @brief
    *    Move file by stream copy and delete
    *
    *  @param[in] dest
    *    Destination file or directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool genericMove(FileHandle & dest);


protected:
    std::unique_ptr<AbstractFileHandleBackend> m_backend; ///< Backend implementation (never null)
};


} // namespace cppfs
