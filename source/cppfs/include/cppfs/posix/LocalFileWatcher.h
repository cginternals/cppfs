
#pragma once


#include <memory>
#include <map>

#include <cppfs/AbstractFileWatcherBackend.h>


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
    *    File watcher that owns the backend
    *  @param[in] fs
    *    File system that created this watcher
    */
    LocalFileWatcher(FileWatcher & fileWatcher, std::shared_ptr<LocalFileSystem> fs);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~LocalFileWatcher();

    // Virtual AbstractFileWatcherBackend functions
    virtual AbstractFileSystem * fs() const override;
    virtual void add(const FileHandle & fileHandle, unsigned int mode) override;
    virtual void watch() override;


protected:
    std::shared_ptr<LocalFileSystem> m_fs;       ///< File system that created this watcher
    int                              m_inotify;  ///< File handle for the inotify instance
    std::map<int, FileHandle>        m_watchers; ///< Map of watch handle -> file handle
};


} // namespace cppfs
