
#include <cppfs/FunctionalFileVisitor.h>

#include <cppfs/FileHandle.h>


namespace cppfs
{


FunctionalFileVisitor::FunctionalFileVisitor()
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFileEntry)
: m_funcFileEntry(std::move(funcFileEntry))
{
}

FunctionalFileVisitor::FunctionalFileVisitor(VisitFunc funcFile, VisitFunc funcDirectory)
: m_funcFile(std::move(funcFile))
, m_funcDirectory(std::move(funcDirectory))
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
