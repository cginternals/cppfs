
#include <cppfs/ssh/SshFileIterator.h>

#include <libssh2.h>

#include <cppfs/ssh/SshFileSystem.h>


namespace cppfs
{


SshFileIterator::SshFileIterator(std::shared_ptr<SshFileSystem> fs, const std::string & path)
: m_fs(fs)
, m_path(path)
, m_dir(nullptr)
, m_index(-1)
{
    // Check handle
    if (!m_fs->m_session) return;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();

    // Check handle
    if (!m_fs->m_sftpSession) return;

    // Open directory
    m_dir = libssh2_sftp_opendir((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str());

    // Read first directory entry
    readNextEntry();
}

SshFileIterator::~SshFileIterator()
{
    if (m_dir)
    {
        libssh2_sftp_closedir(m_dir);
    }
}

std::unique_ptr<AbstractFileIteratorBackend> SshFileIterator::clone() const
{
    auto * twin = new SshFileIterator(m_fs, m_path);

    while (twin->m_index < m_index)
    {
        twin->readNextEntry();
    }

    return std::unique_ptr<AbstractFileIteratorBackend>(twin);
}

AbstractFileSystem * SshFileIterator::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

bool SshFileIterator::valid() const
{
    return (m_dir != nullptr && !m_filename.empty());
}

std::string SshFileIterator::path() const
{
    return m_path;
}

int SshFileIterator::index() const
{
    return m_index;
}

std::string SshFileIterator::name() const
{
    return m_filename;
}

void SshFileIterator::next()
{
    readNextEntry();
}

void SshFileIterator::readNextEntry()
{
    // Check directory handle
    if (!m_dir) return;

    // Read next entry
    char name[512];
    char longName[512];

    int size = libssh2_sftp_readdir_ex(m_dir, name, sizeof(name), longName, sizeof(longName), &m_attrs);
    m_index++;

    if (size <= 0)
    {
        m_filename = "";
        return;
    }

    // Omit '.' and '..'
    m_filename = name;
    while (m_filename == ".." || m_filename == ".")
    {
        size = libssh2_sftp_readdir_ex(m_dir, name, sizeof(name), longName, sizeof(longName), &m_attrs);

        if (size <= 0)
        {
            m_filename = "";
            break;
        }

        m_filename = name;
    }
}


} // namespace cppfs
