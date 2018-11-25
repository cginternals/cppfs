
#pragma once


#include <memory>

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
    virtual void watch() override;


protected:
    std::shared_ptr<LocalFileSystem> m_fs; ///< File system that created this watcher
};


} // namespace cppfs
