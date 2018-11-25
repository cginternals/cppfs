
#pragma once


#include <memory>

#include <cppfs/cppfs.h>


namespace cppfs
{


class FileHandle;
class FileWatcher;
class AbstractFileSystem;


/**
*  @brief
*    Interface for file watchers
*/
class CPPFS_API AbstractFileWatcherBackend
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fileWatcher
    *    File watcher that owns the backend
    */
    AbstractFileWatcherBackend(FileWatcher & fileWatcher);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~AbstractFileWatcherBackend();

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
    *    Watch file handle
    *
    *  @param[in] fileHandle
    *    File handle
    *  @param[in] events
    *    Events that are watched (combination of FileEvent values)
    *  @param[in] recursive
    *    Watch file system recursively? (only relevant if fileHandle points to a directory)
    */
    virtual void add(FileHandle & fileHandle, unsigned int events, RecursiveMode recursive) = 0;

    /**
    *  @brief
    *    Start watching files (blocking)
    *
    *  @remarks
    *    The function is supposed to block until one or more file system
    *    events have occured. For each event, onFileEvent has to be called.
    *    After all events have been processed, the function shall return.
    */
    virtual void watch() = 0;


protected:
    /**
    *  @brief
    *    Called on each file event
    *
    *  @param[in] fileHandle
    *    File handle
    *  @param[in] event
    *    Type of event that has occured
    */
    void onFileEvent(FileHandle & fileHandle, FileEvent event);


protected:
    FileWatcher & m_fileWatcher; ///< File watcher that owns the backend
};


} // namespace cppfs
