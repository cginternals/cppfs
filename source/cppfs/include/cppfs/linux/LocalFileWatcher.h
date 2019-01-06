
#pragma once


#include <memory>
#include <map>

#include <cppfs/AbstractFileWatcherBackend.h>
#include <cppfs/FileHandle.h>


namespace cppfs
{


class LocalFileSystem;


/**
*  @brief
*    File watcher for the local file system
*/
class CPPFS_API LocalFileWatcher : public AbstractFileWatcherBackend
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fileWatcher
    *    File watcher that owns the backend (must NOT be null!)
    *  @param[in] fs
    *    File system that created this watcher
    */
    LocalFileWatcher(FileWatcher * fileWatcher, std::shared_ptr<LocalFileSystem> fs);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~LocalFileWatcher();

    // Virtual AbstractFileWatcherBackend functions
    virtual AbstractFileSystem * fs() const override;
    virtual void add(FileHandle & dir, unsigned int events, RecursiveMode recursive) override;
    virtual void watch(int timeout) override;


protected:
    /**
    *  @brief
    *    Watcher entry
    */
    struct Watcher {
        FileHandle    dir;
        unsigned int  events;
        RecursiveMode recursive;
    };


protected:
    std::shared_ptr<LocalFileSystem> m_fs;       ///< File system that created this watcher
    int                              m_inotify;  ///< File handle for the inotify instance
    std::map<int, Watcher>           m_watchers; ///< Map of watch handle -> file handle
};


} // namespace cppfs
