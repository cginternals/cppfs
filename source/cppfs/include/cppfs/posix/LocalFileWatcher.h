
#pragma once


#include <memory>
#include <vector>

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
    virtual std::unique_ptr<AbstractFileWatcherBackend> clone() const override;
    virtual AbstractFileSystem * fs() const override;
    virtual void add(const std::string & path, unsigned int mode) override;
    virtual void watch() override;


protected:
    std::shared_ptr<LocalFileSystem> m_fs;       ///< File system that created this watcher
    int                              m_inotify;  ///< File handle for the inotify instance
    std::vector<int>                 m_watchers; ///< List of watcher handles
};


} // namespace cppfs
