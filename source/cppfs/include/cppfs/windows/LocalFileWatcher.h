
#pragma once


#include <memory>
#include <mutex>
#include <vector>

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
    virtual void add(FileHandle & fh, unsigned int events, RecursiveMode recursive) override;
    virtual void watch(int timeout) override;


protected:
    /**
    *  @brief
    *    Watcher entry
    */
    struct Watcher {
        std::shared_ptr<void> handle;
        FileHandle            fileHandle;
        unsigned int          events;
        RecursiveMode         recursive;
        std::shared_ptr<void> platform;
    };

protected:
    std::shared_ptr<LocalFileSystem> m_fs;            ///< File system that created this watcher
    std::shared_ptr<void>            m_waitStopEvent; ///< Event to stop watch function
    std::shared_ptr<void>            m_watchersCS;    ///< Watchers critical section
    std::vector<Watcher>             m_watchers;      ///< Watchers
};


} // namespace cppfs
