
#include <cppfs/FileVisitor.h>

#include <cppfs/FileHandle.h>


namespace cppfs
{


FileVisitor::FileVisitor()
{
}

FileVisitor::~FileVisitor()
{
}

void FileVisitor::onFileEntry(FileHandle & fh)
{
    if (fh.isDirectory())
    {
        onDirectory(fh);
    }

    else if (fh.isFile())
    {
        onFile(fh);
    }
}

void FileVisitor::onFile(FileHandle &)
{
}

void FileVisitor::onDirectory(FileHandle &)
{
}


} // namespace cppfs
