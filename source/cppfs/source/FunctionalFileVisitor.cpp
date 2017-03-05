
#include <cppfs/FunctionalFileVisitor.h>

#include <cppfs/FileHandle.h>


namespace cppfs
{


FunctionalFileVisitor::FunctionalFileVisitor()
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFileEntry)
: m_funcFileEntry(funcFileEntry)
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFile, VisitFunc funcDirectory)
: m_funcFile(funcFile)
, m_funcDirectory(funcDirectory)
{
}

FunctionalFileVisitor::~FunctionalFileVisitor()
{
}

bool FunctionalFileVisitor::onFileEntry(FileHandle & fh)
{
    if (m_funcFileEntry)
    {
        return m_funcFileEntry(fh);
    }

    else if (fh.isDirectory())
    {
        return onDirectory(fh);
    }

    else if (fh.isFile())
    {
        return onFile(fh);
    }

    return false;
}

bool FunctionalFileVisitor::onFile(FileHandle & fh)
{
    if (m_funcFile)
    {
        return m_funcFile(fh);
    }

    return false;
}

bool FunctionalFileVisitor::onDirectory(FileHandle & fh)
{
    if (m_funcDirectory)
    {
        return m_funcDirectory(fh);
    }

    return false;
}


} // namespace cppfs
