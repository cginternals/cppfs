
#include <cppfs/FunctionalFileEventHandler.h>


namespace cppfs
{


FunctionalFileEventHandler::FunctionalFileEventHandler()
{
}

FunctionalFileEventHandler::FunctionalFileEventHandler(EventFunc funcFileEvent)
: m_funcFileEvent(std::move(funcFileEvent))
{
}

FunctionalFileEventHandler::~FunctionalFileEventHandler()
{
}

void FunctionalFileEventHandler::onFileEvent(FileHandle & fh, FileEvent event)
{
    if (m_funcFileEvent)
    {
        m_funcFileEvent(fh, event);
    }
}


} // namespace cppfs
