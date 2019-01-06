
#pragma once


#include <memory>
#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


class FileHandle;
class FileWatcher;
class AbstractFileWatcherBackend;


/**
*  @brief
*    Interface for accessing file systems
*/
class CPPFS_API AbstractFileSystem
{
public:
    /**
    *  @brief
    *    Constructor
    */
    AbstractFileSystem();

    /**
    *  @brief
    *    Destructor
    */
    virtual ~AbstractFileSystem();

    /**
    *  @brief
    *    Open file or directory in file system
    *
    *  @param[in] path
    *    Path to file or directory
    */
    virtual FileHandle open(const std::string & path) = 0;

    /**
    *  @brief
    *    Open file or directory in file system
    *
    *  @param[in] path
    *    Path to file or directory
    */
    virtual FileHandle open(std::string && path) = 0;

    /**
    *  @brief
    *    Create a watcher for the file system
    *
    *  @param[in] fileWatcher
    *    File watcher that owns the backend
    *
    *  @return
    *    Watcher backend (must NOT be null!)
    */
    virtual std::unique_ptr<AbstractFileWatcherBackend> createFileWatcher(FileWatcher & fileWatcher) = 0;
};


} // namespace cppfs
