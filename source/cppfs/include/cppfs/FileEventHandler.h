
#pragma once


#include <cppfs/cppfs.h>


namespace cppfs
{


class FileHandle;


/**
*  @brief
*    Handler that is informed about file system events
*
*  @remarks
*    Use FileWatcher to register file event handlers.
*/
class CPPFS_API FileEventHandler
{
    friend class FileWatcher;


public:
    /**
    *  @brief
    *    Constructor
    */
    FileEventHandler();

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FileEventHandler();


protected:
    /**
    *  @brief
    *    Called on file event
    *
    *  @param[in] fh
    *    Handle to file or directory
    *  @param[in] event
    *    Type of event that has occured
    */
    virtual void onFileEvent(FileHandle & fh, FileEvent event);

    /**
    *  @brief
    *    Called when a file or directory has been created
    *
    *  @param[in] fh
    *    Handle to file or directory
    */
    virtual void onFileCreated(FileHandle & fh);

    /**
    *  @brief
    *    Called when a file or directory has been removed
    *
    *  @param[in] fh
    *    Handle to file or directory
    */
    virtual void onFileRemoved(FileHandle & fh);

    /**
    *  @brief
    *    Called when a file or directory has been modified
    *
    *  @param[in] fh
    *    Handle to file or directory
    */
    virtual void onFileModified(FileHandle & fh);

    /**
    *  @brief
    *    Called when file attributes have been modified
    *
    *  @param[in] fh
    *    Handle to file or directory
    */
    virtual void onFileAttrChanged(FileHandle & fh);
};


} // namespace cppfs
