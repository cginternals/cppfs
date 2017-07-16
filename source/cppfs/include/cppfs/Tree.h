
#pragma once


#include <memory>
#include <vector>
#include <string>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


class Diff;


/**
*  @brief
*    Representation of a directory tree
*/
class CPPFS_API Tree
{
public:
    /**
    *  @brief
    *    Constructor
    */
    Tree();

    /**
    *  @brief
    *    Destructor
    */
    ~Tree();

    /**
    *  @brief
    *    Clear data
    */
    void clear();

    /**
    *  @brief
    *    Get path
    *
    *  @return
    *    Path to file or directory
    *
    *  @remarks
    *    The path is constructed relative to the root element
    *    of the tree. So the path of the root element will be
    *    empty (""), the path of the first-order child elements
    *    will equal their filenames, etc.
    */
    const std::string & path() const;

    /**
    *  @brief
    *    Set path
    *
    *  @param[in] path
    *    Path to file or directory
    */
    void setPath(const std::string & path);

    /**
    *  @brief
    *    Set path
    *
    *  @param[in] path
    *    Path to file or directory
    */
    void setPath(std::string && path);

    /**
    *  @brief
    *    Get filename
    *
    *  @return
    *    Filename
    */
    const std::string & fileName() const;

    /**
    *  @brief
    *    Set filename
    *
    *  @param[in] filename
    *    Filename
    */
    void setFileName(const std::string & filename);

    /**
    *  @brief
    *    Set filename
    *
    *  @param[in] filename
    *    Filename
    */
    void setFileName(std::string && filename);

    /**
    *  @brief
    *    Check if item is a file
    *
    *  @return
    *    'true' if it is a file, else 'false'
    */
    bool isFile() const;

    /**
    *  @brief
    *    Check if item is a directory
    *
    *  @return
    *    'true' if it is a directory, else 'false'
    */
    bool isDirectory() const;

    /**
    *  @brief
    *    Set directory
    *
    *  @param[in] isDir
    *    'true' if it is a directory, else 'false'
    */
    void setDirectory(bool isDir);

    /**
    *  @brief
    *    Get file size
    *
    *  @return
    *    Size of file
    */
    unsigned int size() const;

    /**
    *  @brief
    *    Set file size
    *
    *  @param[in] size
    *    Size of file
    */
    void setSize(unsigned int size);

    /**
    *  @brief
    *    Get time of last access
    *
    *  @return
    *    Time stamp
    */
    unsigned int accessTime() const;

    /**
    *  @brief
    *    Set time of last access
    *
    *  @param[in] time
    *    Time stamp
    */
    void setAccessTime(unsigned int time);

    /**
    *  @brief
    *    Get time of last modification
    *
    *  @return
    *    Time stamp
    */
    unsigned int modificationTime() const;

    /**
    *  @brief
    *    Set time of last modification
    *
    *  @param[in] time
    *    Time stamp
    */
    void setModificationTime(unsigned int time);

    /**
    *  @brief
    *    Get ID of owning user
    *
    *  @return
    *    User ID
    */
    unsigned int userId() const;

    /**
    *  @brief
    *    Set ID of owning user
    *
    *  @param[in] id
    *    User ID
    */
    void setUserId(unsigned int id);

    /**
    *  @brief
    *    Get ID of owning group
    *
    *  @return
    *    Group ID
    */
    unsigned int groupId() const;

    /**
    *  @brief
    *    Set ID of owning group
    *
    *  @param[in] id
    *    Group ID
    */
    void setGroupId(unsigned int id);

    /**
    *  @brief
    *    Get file permissions
    *
    *  @return
    *    File permissions
    */
    unsigned long permissions() const;

    /**
    *  @brief
    *    Set file permissions
    *
    *  @param[in] permissions
    *    File permissions
    */
    void setPermissions(unsigned int permissions);

    /**
    *  @brief
    *    Get sha1 hash
    *
    *  @return
    *    SHA1 hash
    */
    const std::string & sha1() const;

    /**
    *  @brief
    *    Set sha1 hash
    *
    *  @param[in] hash
    *    SHA1 hash
    */
    void setSha1(const std::string & hash);

    /**
    *  @brief
    *    Set sha1 hash
    *
    *  @param[in] hash
    *    SHA1 hash
    */
    void setSha1(std::string && hash);

    /**
    *  @brief
    *    List files in directory
    *
    *  @return
    *    List of files, empty list if this is not a valid directory
    */
    std::vector<std::string> listFiles() const;

    /**
    *  @brief
    *    Get children list
    *
    *  @return
    *    List of children
    */
    const std::vector< std::unique_ptr<Tree> > & children() const;

    /**
    *  @brief
    *    Get children list
    *
    *  @return
    *    List of children
    */
    std::vector< std::unique_ptr<Tree> > & children();

    /**
    *  @brief
    *    Add child
    *
    *  @param[in] tree
    *    Child tree (must NOT be null!)
    *
    *  @remarks
    *    The tree takes ownership over the child tree
    */
    void add(std::unique_ptr<Tree> && tree);

    /**
    *  @brief
    *    Print tree to stdout
    *
    *  @param[in] indent
    *    Indentation
    */
    void print(const std::string & indent = "") const;

    /**
    *  @brief
    *    Create diff from this state to target state
    *
    *  @param[in] target
    *    Target tree state
    *
    *  @return
    *    Diff (never null)
    *
    *  @remarks
    *    This functions creates a diff which contains the operations
    *    that are needed to get from this state to the target state.
    *    The returned diff must be deleted by the caller.
    */
    std::unique_ptr<Diff> createDiff(const Tree & target) const;


protected:
    Tree(const Tree &);
    const Tree & operator=(const Tree &);


protected:
    static void createDiff(const Tree * currentState, const Tree * targetState, Diff & diff);


protected:
    std::string   m_path;             ///< Path
    std::string   m_filename;         ///< Filename
    bool          m_directory;        ///< 'true' if directory, 'false' if file
    unsigned int  m_size;             ///< File size
    unsigned int  m_accessTime;       ///< Time of last access
    unsigned int  m_modificationTime; ///< Time of last modification
    unsigned int  m_userId;           ///< User ID
    unsigned int  m_groupId;          ///< Group ID
    unsigned long m_permissions;      ///< File permissions
    std::string   m_sha1;             ///< SHA1 hash

    std::vector< std::unique_ptr<Tree> > m_children; ///< List of children
};


} // namespace cppfs
