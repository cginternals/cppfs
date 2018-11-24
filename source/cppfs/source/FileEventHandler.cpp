
#include <cppfs/FileEventHandler.h>

#include <cppfs/FileHandle.h>


namespace cppfs
{


FileEventHandler::FileEventHandler()
{
}

FileEventHandler::~FileEventHandler()
{
}

void FileEventHandler::onFileEvent(FileHandle & fh, FileEvent event)
{
    if (event & FileCreated) {
        onFileCreated(fh);
    } else if (event & FileRemoved) {
        onFileRemoved(fh);
    } else if (event & FileModified) {
        onFileModified(fh);
    }
}

void FileEventHandler::onFileCreated(FileHandle &)
{
}

void FileEventHandler::onFileRemoved(FileHandle &)
{
}

void FileEventHandler::onFileModified(FileHandle &)
{
}


} // namespace cppfs
