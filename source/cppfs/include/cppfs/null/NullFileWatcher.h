
#pragma once


#include <memory>

#include <cppfs/AbstractFileWatcherBackend.h>


namespace cppfs
{


class AbstractFileSystem;


/**
*  @brief
*    Empty implementation of the file watcher interface
*
*  @remarks
*    This class is used for file systems that do not support
*    file system watchers, such as remote file systems.
*/
class CPPFS_API NullFileWatcher : public AbstractFileWatcherBackend
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fileWatcher
    *    File watcher that owns the backend (must NOT be null!)
    */
    NullFileWatcher(FileWatcher * fileWatcher);

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fileWatcher
    *    File watcher that owns the backend (must NOT be null!)
    *  @param[in] fs
    *    File system that created this watcher
    */
    NullFileWatcher(FileWatcher * fileWatcher, std::shared_ptr<AbstractFileSystem> fs);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~NullFileWatcher();

    // Virtual AbstractFileWatcherBackend functions
    virtual AbstractFileSystem * fs() const override;
    virtual void add(FileHandle & fh, unsigned int events, RecursiveMode recursive) override;
    virtual void watch() override;


protected:
    std::shared_ptr<AbstractFileSystem> m_fs; ///< File system that created this watcher
};


} // namespace cppfs
