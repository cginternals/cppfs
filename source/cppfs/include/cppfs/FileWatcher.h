
#pragma once


#include <vector>
#include <memory>
#include <functional>

#include <cppfs/cppfs.h>
#include <cppfs/AbstractFileWatcherBackend.h>
#include <cppfs/FunctionalFileEventHandler.h>


namespace cppfs
{


class AbstractFileSystem;
class FileHandle;


/**
*  @brief
*    Watcher that reports on changes of files or directories
*/
class CPPFS_API FileWatcher
{
    friend class AbstractFileWatcherBackend;


public:
    /**
    *  @brief
    *    Callback function for file system events
    */
    using EventFunc = std::function<void(FileHandle &, FileEvent)>;


public:
    /**
    *  @brief
    *    Constructor for the local file system
    *
    *  @remarks
    *    Creates a file watcher for the local file system.
    */
    FileWatcher();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fs
    *    Filesystem for which the watcher is created (can be null)
    *
    *  @remarks
    *    If fs == nullptr, the watcher will not be usable with any
    *    file system handles.
    */
    FileWatcher(AbstractFileSystem * fs);

    /**
    *  @brief
    *    Copy constructor (deleted)
    *
    *  @param[in] fileWatcher
    *    Source watcher
    *
    *  @remarks
    *    File watcher objects cannot be copied, as it would make little sense
    *    to duplicate all the watcher tasks and registered callbacks.
    */
    FileWatcher(const FileWatcher & fileWatcher) = delete;

    /**
    *  @brief
    *    Move constructor
    *
    *  @param[in] fileWatcher
    *    Source watcher
    */
    FileWatcher(FileWatcher && fileWatcher);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FileWatcher();

    /**
    *  @brief
    *    Copy operator (deleted)
    *
    *  @param[in] fileWatcher
    *    Source watcher
    *
    *  @remarks
    *    File watcher objects cannot be copied, as it would make little sense
    *    to duplicate all the watcher tasks and registered callbacks.
    */
    FileWatcher & operator=(const FileWatcher & fileWatcher) = delete;

    /**
    *  @brief
    *    Move operator
    *
    *  @param[in] fileWatcher
    *    Source watcher
    */
    FileWatcher & operator=(FileWatcher && fileWatcher);

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
    *    Watch file handle
    *
    *  @param[in] fileHandle
    *    File handle
    *  @param[in] events
    *    Events that are watched (combination of FileEvent values)
    *  @param[in] recursive
    *    Watch file system recursively? (only relevant if fileHandle points to a directory)
    *
    *  @remarks
    *    The file handle must belong to the same file system as the
    *    file watcher, otherwise it will be ignored. Therefore, one
    *    file watcher object can only be used to watch files on a
    *    single file system. Also note that file watching is not
    *    supported for remote file systems, such as SSH.
    */
    void add(const FileHandle & fileHandle, unsigned int events = FileCreated | FileRemoved | FileModified, RecursiveMode recursive = Recursive);

    /**
    *  @brief
    *    Add event handler
    *
    *  @param[in] eventHandler
    *    File event handler (must NOT be null!)
    *
    *  @remarks
    *    Adds the event handler to be called on each file system event.
    */
    void addHandler(FileEventHandler * eventHandler);

    /**
    *  @brief
    *    Add callback function as event handler
    *
    *  @param[in] funcFileEvent
    *    Function that is call on each file system event
    */
    void addHandler(EventFunc funcFileEvent);

    /**
    *  @brief
    *    Remove event handler
    *
    *  @param[in] eventHandler
    *    File event handler (must NOT be null!)
    *
    *  @remarks
    *    Removes the event handler from the watcher.
    */
    void removeHandler(FileEventHandler * eventHandler);

    /**
    *  @brief
    *    Start watching files (blocking)
    *
    *  @remarks
    *    This function begins to watch the file system and blocks until
    *    one or more events have occured. On every event, onFileEvent()
    *    is called with the type of the event and a file handle to the
    *    file or directory. Afterwards, the function returns.
    *    To listen to more events, call watch() again.
    */
    void watch();


protected:
    /**
    *  @brief
    *    Called on file event
    *
    *  @param[in] fileHandle
    *    File handle
    *  @param[in] event
    *    Type of event that has occured
    *
    *  @remarks
    *    The default implementation checks the event and calls
    *    the registered callback functions.
    */
    virtual void onFileEvent(FileHandle & fileHandle, FileEvent event);


protected:
    std::unique_ptr<AbstractFileWatcherBackend> m_backend;       ///< Backend implementation (can be null)
    std::vector<FileEventHandler *>             m_eventHandlers; ///< List of registered file event handlers

    /// Functional event handlers that are owned by the file watcher
    std::vector< std::unique_ptr<FunctionalFileEventHandler> > m_ownEventHandlers;
};


} // namespace cppfs
