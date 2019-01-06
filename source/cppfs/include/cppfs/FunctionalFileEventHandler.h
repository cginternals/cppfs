
#pragma once


#include <functional>

#include <cppfs/FileEventHandler.h>


namespace cppfs
{


/**
*  @brief
*    File event handler that calls a function
*/
class CPPFS_API FunctionalFileEventHandler : public FileEventHandler
{
public:
    /**
    *  @brief
    *    Callback function for file system events
    */
    using EventFunc = std::function<void(FileHandle &, FileEvent)>;


public:
    /**
    *  @brief
    *    Constructor
    */
    FunctionalFileEventHandler();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] funcFileEvent
    *    Function that is call on each file system event
    */
    FunctionalFileEventHandler(EventFunc funcFileEvent);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FunctionalFileEventHandler();


protected:
    virtual void onFileEvent(FileHandle & fh, FileEvent event) override;


protected:
    EventFunc m_funcFileEvent;
};


} // namespace cppfs
