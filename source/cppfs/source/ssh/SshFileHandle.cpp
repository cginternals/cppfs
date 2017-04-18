
#include <cppfs/ssh/SshFileHandle.h>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <cppfs/cppfs.h>
#include <cppfs/FilePath.h>
#include <cppfs/InputStream.h>
#include <cppfs/OutputStream.h>
#include <cppfs/ssh/SshFileSystem.h>
#include <cppfs/ssh/SshFileIterator.h>
#include <cppfs/ssh/SshInputStreamBuffer.h>
#include <cppfs/ssh/SshOutputStreamBuffer.h>


namespace cppfs
{


SshFileHandle::SshFileHandle(std::shared_ptr<SshFileSystem> fs, const std::string & path)
: m_fs(fs)
, m_path(path)
, m_fileInfo(nullptr)
, m_linkInfo(nullptr)
{
}

SshFileHandle::~SshFileHandle()
{
    if (m_fileInfo)
    {
        delete (LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo;
    }

    if (m_linkInfo)
    {
        delete (LIBSSH2_SFTP_ATTRIBUTES *)m_linkInfo;
    }
}

std::unique_ptr<AbstractFileHandleBackend> SshFileHandle::clone() const
{
    return std::unique_ptr<AbstractFileHandleBackend>(new SshFileHandle(m_fs, m_path));
}

AbstractFileSystem * SshFileHandle::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

void SshFileHandle::updateFileInfo()
{
    // Reset file information
    if (m_fileInfo)
    {
        delete (LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo;
        m_fileInfo = nullptr;
    }

    // Reset link information
    if (m_linkInfo)
    {
        delete (LIBSSH2_SFTP_ATTRIBUTES *)m_linkInfo;
        m_linkInfo = nullptr;
    }
}

std::string SshFileHandle::path() const
{
    return m_path;
}

bool SshFileHandle::exists() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        return true;
    }

    // File or directory does not exist
    return false;
}

bool SshFileHandle::isFile() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        // Check if it is a file
        if (((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IFREG)
        {
            return true;
        }
    }

    // File or directory does not exist
    return false;
}

bool SshFileHandle::isDirectory() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        // Check if it is a directory
        if (((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IFDIR)
        {
            return true;
        }
    }

    // File or directory does not exist
    return false;
}

bool SshFileHandle::isSymbolicLink() const
{
    // Get file info
    readLinkInfo();

    // Check if file or directory exists
    if (m_linkInfo)
    {
        // Check if it is a directory
        if (((LIBSSH2_SFTP_ATTRIBUTES *)m_linkInfo)->permissions & LIBSSH2_SFTP_S_IFLNK)
        {
            return true;
        }
    }

    // File or directory does not exist
    return false;
}

std::vector<std::string> SshFileHandle::listFiles() const
{
    std::vector<std::string> entries;

    // Check handle
    if (!m_fs->m_session) return entries;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return entries;

    // Open directory
    LIBSSH2_SFTP_HANDLE * dir = libssh2_sftp_opendir((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str());
    if (!dir) return entries;

    // List entries
    char name[512];
    char longName[512];
    LIBSSH2_SFTP_ATTRIBUTES attrs;

    while(libssh2_sftp_readdir_ex(dir, name, sizeof(name), longName, sizeof(longName), &attrs) > 0)
    {
        std::string filename(name);

        if (filename != "." && filename != "..")
        {
            entries.push_back(filename);
        }
    }

    // Close directory
    libssh2_sftp_closedir(dir);

    // Return directory entries
    return entries;
}

std::unique_ptr<AbstractFileIteratorBackend> SshFileHandle::begin() const
{
    return std::unique_ptr<AbstractFileIteratorBackend>(new SshFileIterator(m_fs, m_path));
}

unsigned int SshFileHandle::size() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        // Check if it is a file
        if (((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IFREG)
        {
            return ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->filesize;
        }
    }

    // Not a file
    return 0;
}

unsigned int SshFileHandle::accessTime() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        return ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->atime;
    }

    return 0;
}

unsigned int SshFileHandle::modificationTime() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        return ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->mtime;
    }

    return 0;
}

unsigned int SshFileHandle::userId() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        return ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->uid;
    }

    return 0;
}

void SshFileHandle::setUserId(unsigned int uid)
{
    // Set user
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    attrs.uid = uid;
    attrs.gid = groupId();

    libssh2_sftp_stat_ex(
        (LIBSSH2_SFTP *)m_fs->m_sftpSession,
        m_path.c_str(),
        m_path.length(),
        LIBSSH2_SFTP_SETSTAT,
        &attrs
    );

    // Invalidate file info
    updateFileInfo();
}

unsigned int SshFileHandle::groupId() const
{
    // Get file info
    readFileInfo();

    // Check if file or directory exists
    if (m_fileInfo)
    {
        return ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->gid;
    }

    return 0;
}

void SshFileHandle::setGroupId(unsigned int gid)
{
    // Set group
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    attrs.flags = LIBSSH2_SFTP_ATTR_UIDGID;
    attrs.uid = userId();
    attrs.gid = gid;

    libssh2_sftp_stat_ex(
        (LIBSSH2_SFTP *)m_fs->m_sftpSession,
        m_path.c_str(),
        m_path.length(),
        LIBSSH2_SFTP_SETSTAT,
        &attrs
    );

    // Invalidate file info
    updateFileInfo();
}

unsigned long SshFileHandle::permissions() const
{
    readFileInfo();

    if (m_fileInfo)
    {
        unsigned long mode = 0;

        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IRUSR )
            mode |= (unsigned long)FilePermissions::UserRead;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IWUSR )
            mode |= (unsigned long)FilePermissions::UserWrite;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IXUSR )
            mode |= (unsigned long)FilePermissions::UserExec;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IRGRP )
            mode |= (unsigned long)FilePermissions::GroupRead;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IWGRP )
            mode |= (unsigned long)FilePermissions::GroupWrite;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IXGRP )
            mode |= (unsigned long)FilePermissions::GroupExec;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IROTH )
            mode |= (unsigned long)FilePermissions::OtherRead;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IWOTH )
            mode |= (unsigned long)FilePermissions::OtherWrite;
        if ( ((LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo)->permissions & LIBSSH2_SFTP_S_IXOTH )
            mode |= (unsigned long)FilePermissions::OtherExec;

        return mode;
    }

    return 0ul;
}

void SshFileHandle::setPermissions(unsigned long permissions)
{
    // Check handle
    if (!m_fs->m_session) return;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return;

    // Convert permission flags
    unsigned long mode = 0;

    if (permissions & (unsigned long)FilePermissions::UserRead)
        mode |= LIBSSH2_SFTP_S_IRUSR;
    if (permissions & (unsigned long)FilePermissions::UserWrite)
        mode |= LIBSSH2_SFTP_S_IWUSR;
    if (permissions & (unsigned long)FilePermissions::UserExec)
        mode |= LIBSSH2_SFTP_S_IXUSR;
    if (permissions & (unsigned long)FilePermissions::GroupRead)
        mode |= LIBSSH2_SFTP_S_IRGRP;
    if (permissions & (unsigned long)FilePermissions::GroupWrite)
        mode |= LIBSSH2_SFTP_S_IWGRP;
    if (permissions & (unsigned long)FilePermissions::GroupExec)
        mode |= LIBSSH2_SFTP_S_IXGRP;
    if (permissions & (unsigned long)FilePermissions::OtherRead)
        mode |= LIBSSH2_SFTP_S_IROTH;
    if (permissions & (unsigned long)FilePermissions::OtherWrite)
        mode |= LIBSSH2_SFTP_S_IWOTH;
    if (permissions & (unsigned long)FilePermissions::OtherExec)
        mode |= LIBSSH2_SFTP_S_IXOTH;

    // Set permissions
    LIBSSH2_SFTP_ATTRIBUTES attrs;
    attrs.flags = LIBSSH2_SFTP_ATTR_PERMISSIONS;
    attrs.permissions = mode;

    libssh2_sftp_stat_ex(
        (LIBSSH2_SFTP *)m_fs->m_sftpSession,
        m_path.c_str(),
        m_path.length(),
        LIBSSH2_SFTP_SETSTAT,
        &attrs
    );

    // Invalidate file info
    updateFileInfo();
}

bool SshFileHandle::createDirectory()
{
    // Check handle
    if (!m_fs->m_session) return false;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return false;

    // Check directory
    if (exists()) return false;

    // Create directory
    if (libssh2_sftp_mkdir((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str(), 0755) != 0)
    {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

bool SshFileHandle::removeDirectory()
{
    // Check handle
    if (!m_fs->m_session) return false;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return false;

    // Check directory
    if (!isDirectory()) return false;

    // Create directory
    if (libssh2_sftp_rmdir((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str()) != 0)
    {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

bool SshFileHandle::copy(AbstractFileHandleBackend & dest)
{
    // This copies a file by executing "cp <src> <dst>" on the remote machine
    // assuming a UNIX system that supports this command. This is of course
    // no ideal, but SCP/SFTP do not have any method to copy files on the remote
    // system without transferring it over network.

    // Check handle
    if (!m_fs->m_session) return false;

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

    // Open channel
    LIBSSH2_CHANNEL * channel = libssh2_channel_open_session((LIBSSH2_SESSION *)m_fs->m_session);
    if (!channel)
    {
        return false;
    }

    // Copy file
    std::string cmd = "cp \"" + src + "\" \"" + dst + "\"";
    if (libssh2_channel_exec(channel, cmd.c_str()) != 0)
    {
        return false;
    }

    // Close channel
    libssh2_channel_free(channel);

    // Done
    updateFileInfo();
    return true;
}

bool SshFileHandle::move(AbstractFileHandleBackend & dest)
{
    // Check handle
    if (!m_fs->m_session) return false;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return false;

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
    if (libssh2_sftp_rename((LIBSSH2_SFTP *)m_fs->m_sftpSession, src.c_str(), dst.c_str()) != 0)
    {
        return false;
    }

    // Update path
    m_path = dst;
    updateFileInfo();

    // Done
    return true;
}

bool SshFileHandle::createLink(AbstractFileHandleBackend &)
{
    // Sorry, not possible with ssh/sftp
    return false;
}

bool SshFileHandle::createSymbolicLink(AbstractFileHandleBackend & dest)
{
    // Check handle
    if (!m_fs->m_session) return false;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return false;

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
    if (libssh2_sftp_symlink((LIBSSH2_SFTP *)m_fs->m_sftpSession, src.c_str(), const_cast<char *>(dst.c_str())) != 0)
    {
        return false;
    }

    // Done
    return true;
}

bool SshFileHandle::rename(const std::string & filename)
{
    // Check handle
    if (!m_fs->m_session) return false;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return false;

    // Check file
    if (!exists()) return false;

    // Compose new file path
    std::string path = FilePath(FilePath(m_path).directoryPath()).resolve(filename).fullPath();

    // Rename
    if (libssh2_sftp_rename((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str(), path.c_str()) != 0)
    {
        return false;
    }

    // Update path
    m_path = path;
    updateFileInfo();

    // Done
    return true;
}

bool SshFileHandle::remove()
{
    // Check handle
    if (!m_fs->m_session) return false;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return false;

    // Check source file
    if (!isFile()) return false;

    // Delete file
    if (libssh2_sftp_unlink((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str()) != 0)
    {
        return false;
    }

    // Done
    updateFileInfo();
    return true;
}

std::unique_ptr<std::istream> SshFileHandle::createInputStream(std::ios_base::openmode mode) const
{
    // Check handle
    if (!m_fs->m_session) return nullptr;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return nullptr;

    // Check source file
    if (!isFile()) return nullptr;

    // Create stream
    return std::unique_ptr<std::istream>(
        new InputStream(new SshInputStreamBuffer(m_fs, m_path, mode))
    );
}

std::unique_ptr<std::ostream> SshFileHandle::createOutputStream(std::ios_base::openmode mode)
{
    // Check handle
    if (!m_fs->m_session) return nullptr;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return nullptr;

    // Create stream
    return std::unique_ptr<std::ostream>(
        new OutputStream(new SshOutputStreamBuffer(m_fs, m_path, mode))
    );
}

void SshFileHandle::readFileInfo() const
{
    // Check if file info has already been read
    if (m_fileInfo) return;

    // Check handle
    if (!m_fs->m_session) return;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return;

    // Create file information structure
    m_fileInfo = (void *)new LIBSSH2_SFTP_ATTRIBUTES;

    // Get file info
    if (libssh2_sftp_stat_ex((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str(), m_path.length(), LIBSSH2_SFTP_STAT, (LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo) != 0)
    {
        // Error!
        delete (LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo;
        m_fileInfo = nullptr;
    }
}

void SshFileHandle::readLinkInfo() const
{
    // Check if file info has already been read
    if (m_linkInfo) return;

    // Check handle
    if (!m_fs->m_session) return;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();
    if (!m_fs->m_sftpSession) return;

    // Create file information structure
    m_linkInfo = (void *)new LIBSSH2_SFTP_ATTRIBUTES;

    // Get file info
    if (libssh2_sftp_stat_ex((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str(), m_path.length(), LIBSSH2_SFTP_LSTAT, (LIBSSH2_SFTP_ATTRIBUTES *)m_fileInfo) != 0)
    {
        // Error!
        delete (LIBSSH2_SFTP_ATTRIBUTES *)m_linkInfo;
        m_linkInfo = nullptr;
    }
}


} // namespace cppfs
