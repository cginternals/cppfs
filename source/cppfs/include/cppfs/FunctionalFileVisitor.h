
#pragma once


#include <functional>

#include <cppfs/FileVisitor.h>


namespace cppfs
{


/**
*  @brief
*    File visitor that calls a function
*/
class CPPFS_API FunctionalFileVisitor : public FileVisitor
{
public:
    using VisitFunc = std::function<bool(FileHandle &)>;


public:
    /**
    *  @brief
    *    Constructor
    */
    FunctionalFileVisitor();

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] funcFileEntry
    *    Function that is call on each file entry (files and directories)
    */
    FunctionalFileVisitor(VisitFunc funcFileEntry);

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] funcFile
    *    Function that is call on each file
    *  @param[in] funcDirectory
    *    Function that is call on each directory
    */
    FunctionalFileVisitor(VisitFunc funcFile, VisitFunc funcDirectory);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FunctionalFileVisitor();


protected:
    virtual bool onFileEntry(FileHandle & fh) override;
    virtual bool onFile(FileHandle & fh) override;
    virtual bool onDirectory(FileHandle & fh) override;


protected:
    VisitFunc m_funcFileEntry;
    VisitFunc m_funcFile;
    VisitFunc m_funcDirectory;
};


} // namespace cppfs
