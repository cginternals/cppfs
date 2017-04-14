
#include <cppfs/posix/LocalFileHandle.h>

#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cppfs/cppfs.h>
#include <cppfs/FilePath.h>
#include <cppfs/posix/LocalFileSystem.h>
#include <cppfs/posix/LocalFileIterator.h>


namespace cppfs
{


LocalFileHandle::LocalFileHandle(std::shared_ptr<LocalFileSystem> fs, const std::string & path)
: LocalFileHandle(fs, std::string(path))
{
}

LocalFileHandle::LocalFileHandle(std::shared_ptr<LocalFileSystem> fs, std::string && path)
: m_fs(fs)
, m_path(path)
, m_fileInfo(nullptr)
, m_linkInfo(nullptr)
{
}

LocalFileHandle::~LocalFileHandle()
{
    if (m_fileInfo)
    {
        delete (struct stat *)m_fileInfo;
    }

    if (m_linkInfo)
    {
        delete (struct stat *)m_linkInfo;
    }
}

std::unique_ptr<AbstractFileHandleBackend> LocalFileHandle::clone() const
{
    return std::unique_ptr<AbstractFileHandleBackend>(new LocalFileHandle(m_fs, m_path));
}

AbstractFileSystem * LocalFileHandle::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void LocalFileHandle::updateFileInfo()
{
    // Reset file information
    if (m_fileInfo)
    {
        delete (struct stat *)m_fileInfo;
        m_fileInfo = nullptr;
    }

    // Reset link information
    if (m_linkInfo)
    {
        delete (struct stat *)m_linkInfo;
        m_linkInfo = nullptr;
    }
}

std::string LocalFileHandle::path() const
{
    return m_path;
}

bool LocalFileHandle::exists() const
{
    readFileInfo();

    return (m_fileInfo != nullptr);
}

bool LocalFileHandle::isFile() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        return S_ISREG( ((struct stat *)m_fileInfo)->st_mode );
    }

    return false;
}

bool LocalFileHandle::isDirectory() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        return S_ISDIR( ((struct stat *)m_fileInfo)->st_mode );
    }

    return false;
}

bool LocalFileHandle::isSymbolicLink() const
{
    readLinkInfo();

    if (m_linkInfo)
    {
        return S_ISLNK( ((struct stat *)m_linkInfo)->st_mode );
    }

    return false;
}

std::vector<std::string> LocalFileHandle::listFiles() const
{
    std::vector<std::string> entries;

    // Open directory
    DIR * dir = opendir(m_path.c_str());
    if (!dir)
    {
        return entries;
    }

    // Read directory entries
    struct dirent * entry = readdir(dir);
    while (entry)
    {
        // Get name
        std::string name = entry->d_name;

        // Ignore . and ..
        if (name != ".." && name != ".")
        {
            entries.push_back(name);
        }

        // Next entry
        entry = readdir(dir);
    }

    // Close directory
    closedir(dir);

    // Return directory entries
    return entries;
}

std::unique_ptr<AbstractFileIteratorBackend> LocalFileHandle::begin() const
{
    return std::unique_ptr<AbstractFileIteratorBackend>(new LocalFileIterator(m_fs, m_path));
}

unsigned int LocalFileHandle::size() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        if (S_ISREG( ((struct stat *)m_fileInfo)->st_mode ))
        {
            return ((struct stat *)m_fileInfo)->st_size;
        }
    }

    return 0;
}

unsigned int LocalFileHandle::accessTime() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        return ((struct stat *)m_fileInfo)->st_atime;
    }

    return 0;
}

unsigned int LocalFileHandle::modificationTime() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        return ((struct stat *)m_fileInfo)->st_mtime;
    }

    return 0;
}

unsigned int LocalFileHandle::userId() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        return ((struct stat *)m_fileInfo)->st_uid;
    }

    return 0;
}

void LocalFileHandle::setUserId(unsigned int uid)
{
    // Set user and group
    if (chown(m_path.c_str(), uid, groupId()) == 0)
    {
    }

    // Invalidate file info
    updateFileInfo();
}

unsigned int LocalFileHandle::groupId() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        return ((struct stat *)m_fileInfo)->st_gid;
    }

    return 0;
}

void LocalFileHandle::setGroupId(unsigned int gid)
{
    // Set user and group
    if (chown(m_path.c_str(), userId(), gid) == 0)
    {
    }

    // Invalidate file info
    updateFileInfo();
}

unsigned long LocalFileHandle::permissions() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        unsigned long mode = 0;

        if ( ((struct stat *)m_fileInfo)->st_mode & S_IRUSR )
            mode |= (unsigned long)FilePermissions::UserRead;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IWUSR )
            mode |= (unsigned long)FilePermissions::UserWrite;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IXUSR )
            mode |= (unsigned long)FilePermissions::UserExec;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IRGRP )
            mode |= (unsigned long)FilePermissions::GroupRead;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IWGRP )
            mode |= (unsigned long)FilePermissions::GroupWrite;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IXGRP )
            mode |= (unsigned long)FilePermissions::GroupExec;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IROTH )
            mode |= (unsigned long)FilePermissions::OtherRead;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IWOTH )
            mode |= (unsigned long)FilePermissions::OtherWrite;
        if ( ((struct stat *)m_fileInfo)->st_mode & S_IXOTH )
            mode |= (unsigned long)FilePermissions::OtherExec;

        return mode;
    }

    return 0ul;
}

void LocalFileHandle::setPermissions(unsigned long permissions)
{
    // Convert permission flags
    unsigned long mode = 0;

    if (permissions & (unsigned long)FilePermissions::UserRead)
        mode |= S_IRUSR;
    if (permissions & (unsigned long)FilePermissions::UserWrite)
        mode |= S_IWUSR;
    if (permissions & (unsigned long)FilePermissions::UserExec)
        mode |= S_IXUSR;
    if (permissions & (unsigned long)FilePermissions::GroupRead)
        mode |= S_IRGRP;
    if (permissions & (unsigned long)FilePermissions::GroupWrite)
        mode |= S_IWGRP;
    if (permissions & (unsigned long)FilePermissions::GroupExec)
        mode |= S_IXGRP;
    if (permissions & (unsigned long)FilePermissions::OtherRead)
        mode |= S_IROTH;
    if (permissions & (unsigned long)FilePermissions::OtherWrite)
        mode |= S_IWOTH;
    if (permissions & (unsigned long)FilePermissions::OtherExec)
        mode |= S_IXOTH;

    // Set permissions
    chmod(m_path.c_str(), mode);

    // Invalidate file info
    updateFileInfo();
}

bool LocalFileHandle::createDirectory()
{
    // Check directory
    if (exists()) return false;

    // Create directory
    if (::mkdir(m_path.c_str(), 0755) != 0)
    {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

bool LocalFileHandle::removeDirectory()
{
    // Check directory
    if (!isDirectory()) return false;

    // Remove directory
    if (::remove(m_path.c_str()) != 0)
    {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

bool LocalFileHandle::copy(AbstractFileHandleBackend & dest)
{
    // Check source file
    if (!isFile()) return false;

    // Get source and target filenames
    std::string src = m_path;
    std::string dst = dest.path();

    if (dest.isDirectory())
    {
        std::string filename = FilePath(m_path).fileName();
        dst = FilePath(dest.path()).resolve(filename).fullPath();
    }

    // Open files
    std::ifstream in (src.c_str(), std::ios::binary);
    std::ofstream out(dst.c_str(), std::ios::binary);
    if (!in || !out)
    {
        // Error!
        return false;
    }

    // Copy file
    out << in.rdbuf();

    // Done
    return true;
}

bool LocalFileHandle::move(AbstractFileHandleBackend & dest)
{
    // Check source file
    if (!exists()) return false;

    // Get source and target filenames
    std::string src = m_path;
    std::string dst = dest.path();

    if (dest.isDirectory())
    {
        std::string filename = FilePath(m_path).fileName();
        dst = FilePath(dest.path()).resolve(filename).fullPath();
    }

    // Move file
    if (::rename(src.c_str(), dst.c_str()) != 0)
    {
        return false;
    }

    // Update path
    m_path = dst;
    updateFileInfo();

    // Done
    return true;
}

bool LocalFileHandle::createLink(AbstractFileHandleBackend & dest)
{
    // Check source file
    if (!exists()) return false;

    // Get source and target filenames
    std::string src = m_path;
    std::string dst = dest.path();

    if (dest.isDirectory())
    {
        std::string filename = FilePath(m_path).fileName();
        dst = FilePath(dest.path()).resolve(filename).fullPath();
    }

    // Create link
    if (::link(src.c_str(), dst.c_str()) != 0)
    {
        return false;
    }

    // Done
    return true;
}

bool LocalFileHandle::createSymbolicLink(AbstractFileHandleBackend & dest)
{
    // Check source file
    if (!exists()) return false;

    // Get source and target filenames
    std::string src = m_path;
    std::string dst = dest.path();

    if (dest.isDirectory())
    {
        std::string filename = FilePath(m_path).fileName();
        dst = FilePath(dest.path()).resolve(filename).fullPath();
    }

    // Create symbolic link
    if (::symlink(src.c_str(), dst.c_str()) != 0)
    {
        return false;
    }

    // Done
    return true;
}

bool LocalFileHandle::rename(const std::string & filename)
{
    // Check file
    if (!exists()) return false;

    // Compose new file path
    std::string path = FilePath(FilePath(m_path).directoryPath()).resolve(filename).fullPath();

    // Rename
    if (::rename(m_path.c_str(), path.c_str()) != 0)
    {
        return false;
    }

    // Update path
    m_path = path;
    updateFileInfo();

    // Done
    return true;
}

bool LocalFileHandle::remove()
{
    // Check source file
    if (!isFile()) return false;

    // Delete file
    if (::remove(m_path.c_str()) != 0)
    {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

std::unique_ptr<std::istream> LocalFileHandle::createInputStream(std::ios_base::openmode mode) const
{
    return std::unique_ptr<std::istream>(new std::ifstream(m_path, mode));
}

std::unique_ptr<std::ostream> LocalFileHandle::createOutputStream(std::ios_base::openmode mode)
{
    return std::unique_ptr<std::ostream>(new std::ofstream(m_path, mode));
}

void LocalFileHandle::readFileInfo() const
{
    // Check if file info has already been read
    if (m_fileInfo) return;

    // Create file information structure
    m_fileInfo = (void *)new struct stat;

    // Get file info
    if (stat(m_path.c_str(), (struct stat *)m_fileInfo) != 0)
    {
        // Error!
        delete (struct stat *)m_fileInfo;
        m_fileInfo = nullptr;
    }
}

void LocalFileHandle::readLinkInfo() const
{
    // Check if file info has already been read
    if (m_linkInfo) return;

    // Create file information structure
    m_linkInfo = (void *)new struct stat;

    // Get file info
    if (lstat(m_path.c_str(), (struct stat *)m_linkInfo) != 0)
    {
        // Error!
        delete (struct stat *)m_linkInfo;
        m_linkInfo = nullptr;
    }
}


} // namespace cppfs
