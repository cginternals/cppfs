
#pragma once


#include <memory>

#include <cppfs/cppfs.h>


namespace cppfs
{


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
    *    Create a copy of this watcher
    *
    *  @return
    *    File watcher
    */
    virtual std::unique_ptr<AbstractFileWatcherBackend> clone() const = 0;

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
    *  @param[in] path
    *    File path
    *  @param[in] mode
    *    Watch mode (combination of FileEvent values)
    */
    virtual void add(const std::string & path, unsigned int mode) = 0;

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
    *  @param[in] path
    *    Path to file or directory
    *  @param[in] event
    *    Type of event that has occured
    */
    void onFileEvent(const std::string & path, FileEvent event);


protected:
    FileWatcher & m_fileWatcher; ///< File watcher that owns the backend
};


} // namespace cppfs
