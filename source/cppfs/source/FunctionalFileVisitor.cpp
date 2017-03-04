
#include <cppfs/FunctionalFileVisitor.h>

#include <cppfs/FileHandle.h>


namespace cppfs
{


FunctionalFileVisitor::FunctionalFileVisitor()
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFileEntry, VisitFunc funcFile, VisitFunc funcDirectory)
: m_funcFileEntry(funcFileEntry)
, m_funcFile(funcFile)
, m_funcDirectory(funcDirectory)
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFile, VisitFunc funcDirectory)
: m_funcFile(funcFile)
, m_funcDirectory(funcDirectory)
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFileEntry)
: m_funcFileEntry(funcFileEntry)
{
}

FunctionalFileVisitor::~FunctionalFileVisitor()
{
}

void FunctionalFileVisitor::onFileEntry(FileHandle & fh)
{
    if (m_funcFileEntry)
    {
        m_funcFileEntry(fh);
    }

    if (fh.isDirectory())
    {
        onDirectory(fh);
    }

    else if (fh.isFile())
    {
        onFile(fh);
    }
}

void FunctionalFileVisitor::onFile(FileHandle & fh)
{
    if (m_funcFile)
    {
        m_funcFile(fh);
    }
}

void FunctionalFileVisitor::onDirectory(FileHandle & fh)
{
    if (m_funcDirectory)
    {
        m_funcDirectory(fh);
    }
}


} // namespace cppfs
