
#pragma once


#include <memory>
#include <mutex>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
        FileHandle            dir;           ///< Directory that is watched
        unsigned int          events;        ///< Watched events
        RecursiveMode         recursive;     ///< Watch recursively?
        std::shared_ptr<void> dirHandle;     ///< Handle for the directory
        std::shared_ptr<void> event;         ///< Event that is triggered for this watcher
        ::OVERLAPPED          overlapped;    ///< Overlapped data (for asynchronous operation)
        char                  buffer[16384]; ///< Buffer for overlapped data (1024 * sizeof(FILE_NOTIFY_INFORMATION))
    };


protected:
    std::shared_ptr<LocalFileSystem> m_fs;            ///< File system that created this watcher
    std::vector<Watcher>             m_watchers;      ///< List of watchers
    ::HANDLE                         m_waitStopEvent; ///< Event to stop watch function
    ::CRITICAL_SECTION               m_mutexWatchers; ///< Mutex/critical section for accessing m_watchers
};


} // namespace cppfs
