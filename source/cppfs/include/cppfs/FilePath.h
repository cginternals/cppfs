
#pragma once


#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    Path to file or directory
*
*    This class stores a path to a file or directory and provides common
*    operations like getting the file name or extension.
*
*    FilePath uses a unified format for storing paths that can be used
*    consistently on every platform, using only '/' as a separator.
*    When a FilePath is constructed from a string, the path is translated into
*    the unified format and can then be used in a cross-platform way throughout
*    an application. All operations on FilePath also return paths in the same
*    unified format. To obtain a path in native platform format, use toNative().
*
*    All operations are completely string-based and don't use any system information.
*    Therefore, paths are treated purely syntactically and do not imply that
*    for example a file does really exist.
*/
class CPPFS_API FilePath
{
public:
    /**
    *  @brief
    *    Constructor
    */
    FilePath();

    /**
    *  @brief
    *    Copy constructor
    *
    *  @param[in] filePath
    *    File path to copy
    */
    FilePath(const FilePath & filePath);

    /**
    *  @brief
    *    Move constructor
    *
    *  @param[in] filePath
    *    File path to move
    */
    FilePath(FilePath && filePath);

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] path
    *    File path
    */
    FilePath(const std::string & path);

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] path
    *    File path
    */
    FilePath(std::string && path);

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] string
    *    File path
    */
    FilePath(const char * path);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~FilePath();

    /**
    *  @brief
    *    Copy assignment operator
    *
    *  @param[in] filePath
    *    Right-hand value to copy
    *
    *  @return
    *    Reference to this value
    */
    FilePath & operator=(const FilePath & filePath);

    /**
    *  @brief
    *    Move assignment operator
    *
    *  @param[in] filePath
    *    Right-hand value to move
    *
    *  @return
    *    Reference to this value
    */
    FilePath & operator=(FilePath && filePath);

    /**
    *  @brief
    *    Get path as string
    *
    *  @return
    *    File path (unified format)
    *
    *  @remarks
    *    The path will be returned in unified format, but otherwise unchanged
    *    (e.g., trailing separators are preserved). To get the path without
    *    trailing separators, use fullPath().
    */
    const std::string & path() const;

    /**
    *  @brief
    *    Set path
    *
    *  @param[in] path
    *    File path
    */
    void setPath(const std::string & path);

    /**
    *  @brief
    *    Set path
    *
    *  @param[in] path
    *    File path
    */
    void setPath(std::string && path);

    /**
    *  @brief
    *    Get native path as string
    *
    *  @return
    *    File path (native format)
    *
    *  @remarks
    *    The path will be returned in native format, but otherwise unchanged
    *    (e.g., trailing separators are preserved).
    */
    std::string toNative() const;

    /**
    *  @brief
    *    Check if path is empty
    *
    *  @return
    *    'true' if path is empty, else 'false'
    */
    bool isEmpty() const;

    /**
    *  @brief
    *    Check if path points to the contents of a directory or container
    *
    *  @return
    *    'true' if it points to the contents, else 'false'
    *
    *  @remarks
    *    A path can either end with a filename (e.g., '/path/to/dir'),
    *    or with a separator (e.g., '/path/to/dir/'). For files, there
    *    is no difference in both. For directories however, it is
    *    assumed that a path that ends with a separator points to the
    *    contents of the directory rather than the directory itself.
    *    The same may be true for containers, such as archive files.
    *    This function can therefore be used to determine if the path
    *    ends with a separator, yet it makes no difference between
    *    files and directories (as it can operate only on the string).
    */
    bool pointsToContent() const;

    /**
    *  @brief
    *    Get full path
    *
    *  @return
    *    Full path in unified format, but without a trailing separator
    *
    *  @remarks
    *    If you want trailing separators to remain in the string, use path().
    *    Calling this function triggers a full analysis of the path (costly operation).
    */
    const std::string & fullPath() const;

    /**
    *  @brief
    *    Get file name
    *
    *  @return
    *    File name of the stored path (with extension)
    *
    *  @remarks
    *    This function returns "something.ex" for both "/path/to/something.ex" and
    *    "/path/to/something.ex/".
    *    Calling this function triggers a full analysis of the path (costly operation).
    */
    const std::string & fileName() const;

    /**
    *  @brief
    *    Get base name
    *
    *  @return
    *    Base name of the stored path (without extension)
    *
    *  @remarks
    *    This function returns "something" for both "/path/to/something.ex" and
    *    "/path/to/something.ex/".
    *    Calling this function triggers a full analysis of the path (costly operation).
    */
    const std::string & baseName() const;

    /**
    *  @brief
    *    Get file extension
    *
    *  @return
    *    Extension of the stored path
    *
    *  @remarks
    *    This function returns ".ex" for both "/path/to/something.ex" and
    *    "/path/to/something.ex/". If the path has no extension, an empty
    *    string is returned.
    *    Calling this function triggers a full analysis of the path (costly operation).
    */
    const std::string & extension() const;

    /**
    *  @brief
    *    Get directory path
    *
    *  @return
    *    Path to the directory, with trailing slashes
    *
    *  @remarks
    *    This function returns "/path/to/" as directory path for both
    *    "/path/to/directory" and "/path/to/directory/".
    *    Calling this function triggers a full analysis of the path (costly operation).
    */
    const std::string & directoryPath() const;

    /**
    *  @brief
    *    Get drive letter
    *
    *  @return
    *    Drive letter of the path on Windows (e.g., "C:").
    *
    *  @remarks
    *    If there is no drive letter (Linux, Mac), an empty string is returned.
    *    Calling this function triggers a full analysis of the path (costly operation).
    */
    const std::string & driveLetter() const;

    /**
    *  @brief
    *    Check if path is absolute
    *
    *  @return
    *    'true' if path is absolute, else 'false'
    */
    bool isAbsolute() const;

    /**
    *  @brief
    *    Check if path is relative
    *
    *  @return
    *    'true' if path is relative, else 'false'
    */
    bool isRelative() const;

    /**
    *  @brief
    *    Resolve relative path from this path
    *
    *  @param[in] path
    *    Path
    *
    *  @return
    *    Combined path
    *
    *  @remarks
    *    This function applies the specified path to the
    *    current path of this object. It uses the following
    *    rules:
    *    - If path is absolute, path is returned
    *    - If path is relative, it is appended to this path
    *    - If path ends with a separator, the new path also will
    *    - Trailing separators of this path are not preserved when adding path
    */
    FilePath resolve(const FilePath & path) const;

    /**
    *  @brief
    *    Resolve path (removed '.' and '..' entries if possible)
    *
    *  @param[in] path
    *    Path
    *
    *  @return
    *    Resolved path
    *
    *  @remarks
    *    This function resolves occurences of '.' and '..', taking into
    *    account the root path, e.g.:
    *      'a/b/../c' -> 'a/c'
    *      '../../a' -> '../../a'
    *      'a/../../b' -> '../b'
    *      '/a/../b/' - '/b'
    *      etc.
    */
    std::string resolved() const;


protected:
    /**
    *  @brief
    *    Analyze path and fill in the additional information
    *
    *  @remarks
    *    This function will populate additional information,
    *    such as m_filename, etc. and set m_details to 'true'.
    *    If m_details is already 'true', it returns immediately.
    */
    void analyze() const;


protected:
    std::string m_path;                  ///< Path (unified format)
    bool        m_pointsToContent;       ///< 'true' if the path has a trailing separator, else 'false'

    mutable bool        m_details;       ///< 'true' if path details have been analyzed, else 'false'
    mutable std::string m_fullPath;      ///< Full path (without trailing separators)
    mutable std::string m_filename;      ///< Filename component
    mutable std::string m_basename;      ///< Basename component
    mutable std::string m_extension;     ///< Extension component
    mutable std::string m_directoryPath; ///< Path to containing directory
    mutable std::string m_driveLetter;   ///< Drive letter component
    mutable bool        m_absolute;      ///< 'true' if path is absolute, else 'false'
};


} // namespace cppfs
