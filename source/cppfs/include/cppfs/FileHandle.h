
#pragma once


#include <memory>
#include <ios>
#include <functional>
#include <vector>
#include <string>

#include <cppfs/cppfs.h>
#include <cppfs/AbstractFileHandleBackend.h>


namespace cppfs
{


class AbstractFileSystem;
class FileIterator;
class FileVisitor;
class Tree;
class FileWatcher;


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
    using VisitFunc = std::function<bool(FileHandle &)>;


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
    FileHandle(std::unique_ptr<AbstractFileHandleBackend> && backend);

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
    *    Move operator
    *
    *  @param[in] fileHandle
    *    Source handle
    */
    FileHandle & operator=(FileHandle && fileHandle);

    /**
    *  @brief
    *    Get file system
    *
    *  @return
    *    File system (can be null)
    */
    AbstractFileSystem * fs() const;

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
    *    Check if item is a symbolic link
    *
    *  @return
    *    'true' if it is a symbolic link, else 'false'
    */
    bool isSymbolicLink() const;

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
    *    Traverse directory tree with callback functions
    *
    *  @param[in] funcFileEntry
    *    Function that is call on each file entry (files and directories)
    */
    void traverse(VisitFunc funcFileEntry);

    /**
    *  @brief
    *    Traverse directory tree with callback functions
    *
    *  @param[in] funcFile
    *    Function that is call on each file
    *  @param[in] funcDirectory
    *    Function that is call on each directory
    */
    void traverse(VisitFunc funcFile, VisitFunc funcDirectory);

    /**
    *  @brief
    *    Traverse directory tree with a visitor
    *
    *  @param[in] visitor
    *    Visitor that is invoked for each entry in the directory tree
    */
    void traverse(FileVisitor & visitor);

    /**
    *  @brief
    *    Read directory tree
    *
    *  @param[in] path
    *    File path for the root element
    *  @param[in] includeHash
    *    Compute SHA1 hash of each file? (slow, as each will must be read entirely)
    *
    *  @return
    *    File tree, nullptr if this file does not exist
    */
    std::unique_ptr<Tree> readTree(const std::string & path = "", bool includeHash = false) const;

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
    *    Compute sha1 hash for file
    *
    *  @return
    *    SHA1 hash, "" on error
    */
    std::string sha1() const;

    /**
    *  @brief
    *    Get base64 encoded file content
    *
    *  @return
    *    Base64 encoded file, "" on error
    */
    std::string base64() const;

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
    FileHandle open(const std::string & path) const;

    /**
    *  @brief
    *    Create directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool createDirectory();

    /**
    *  @brief
    *    Remove directory
    *
    *  @return
    *    'true' if successful, else 'false'
    *
    *  @remarks
    *    Only works if the directory exists and is empty.
    *    To remove an entire directory tree, see removeDirectoryRec.
    */
    bool removeDirectory();

    /**
    *  @brief
    *    Copy directory recursively
    *
    *  @param[in] dstDir
    *    Destination directory
    *
    *  @remarks
    *    Destination directory points to the actual directory that
    *    is to be created, not its parent!
    *
    *    Example:
    *      FileHandle dir = fs::open("/projects/project1");
    *      dir.copyDirectory(fs::open("/backup/projects/project1"))
    */
    void copyDirectoryRec(FileHandle & dstDir);

    /**
    *  @brief
    *    Remove directory recursively
    */
    void removeDirectoryRec();

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
    *    Create hard link
    *
    *  @param[in] dest
    *    Destination file or directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool createLink(FileHandle & dest);

    /**
    *  @brief
    *    Create symbolic link
    *
    *  @param[in] dest
    *    Destination file or directory
    *
    *  @return
    *    'true' if successful, else 'false'
    */
    bool createSymbolicLink(FileHandle & dest);

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
    *    Create file system watcher for this file handle
    *
    *  @param[in] events
    *    Events that are watched (combination of FileEvent values)
    *  @param[in] recursive
    *    Watch file system recursively?
    *
    *  @return
    *    File watcher
    *
    *  @remarks
    *    This is a shortcut for creating a FileWatcher and adding file handles to watch.
    *    It will only work if the file handle points to a valid directory.
    *    To watch more than one directory at a time, use FileWatcher and add.
    *    Avoid creating more than one FileWatcher, as OS limits can be reached.
    */
    FileWatcher watch(unsigned int events = FileCreated | FileRemoved | FileModified | FileAttrChanged, RecursiveMode recursive = Recursive);

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
    std::unique_ptr<std::istream> createInputStream(std::ios_base::openmode mode = std::ios_base::in) const;

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
    std::unique_ptr<std::ostream> createOutputStream(std::ios_base::openmode mode = std::ios_base::out);

    /**
    *  @brief
    *    Read file to string
    *
    *  @return
    *    File content, "" on error
    */
    std::string readFile() const;

    /**
    *  @brief
    *    Write string to file
    *
    *  @param[in] content
    *    File content
    *
    *  @return
    *    'true' on success, else 'false'
    */
    bool writeFile(const std::string & content);

    /**
    *  @brief
    *    Write file content from base64 encoded string
    *
    *  @param[in] base64
    *    File content in base64 encoding
    *
    *  @return
    *    'true' on success, else 'false'
    */
    bool writeFileBase64(const std::string & base64);


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
    std::unique_ptr<AbstractFileHandleBackend> m_backend; ///< Backend implementation (can be null)
};


} // namespace cppfs
