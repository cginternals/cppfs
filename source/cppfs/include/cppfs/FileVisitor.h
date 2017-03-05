
#pragma once


#include <cppfs/cppfs_api.h>


namespace cppfs
{


class FileHandle;


/**
*  @brief
*    File visitor
*/
class CPPFS_API FileVisitor
{
    friend class FileHandle;


public:
    /**
    *  @brief
    *    Constructor
    */
    FileVisitor();

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FileVisitor();


protected:
    /**
    *  @brief
    *    Called on each file entry (files and directories)
    *
    *  @param[in] fh
    *    Handle to file or directory
    *
    *  @remarks
    *    The default implementation checks if the file handle
    *    points to a file or a directory and calls onFile()
    *    or onDirectory() respectively.
    *
    *  @return
    *    'true' if subdirectory should be traversed, else 'false'
    */
    virtual bool onFileEntry(FileHandle & fh);

    /**
    *  @brief
    *    Called on each file
    *
    *  @param[in] fh
    *    Handle to file
    *
    *  @return
    *    Irrelevant for files
    */
    virtual bool onFile(FileHandle & fh);

    /**
    *  @brief
    *    Called on each directory
    *
    *  @param[in] fh
    *    Handle to directory
    *
    *  @return
    *    'true' if subdirectory should be traversed, else 'false'
    */
    virtual bool onDirectory(FileHandle & fh);
};


} // namespace cppfs
