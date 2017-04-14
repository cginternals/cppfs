
#include <cppfs/ssh/SshOutputStreamBuffer.h>

#include <algorithm>
#include <iostream>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <cppfs/ssh/SshFileSystem.h>


#ifdef max
	#undef max
#endif


namespace cppfs
{


SshOutputStreamBuffer::SshOutputStreamBuffer(std::shared_ptr<SshFileSystem> fs, const std::string & path, std::ios_base::openmode mode, size_t bufferSize)
: m_fs(fs)
, m_path(path)
, m_file(nullptr)
, m_buffer(std::max(bufferSize, (size_t)1))
{
    // Check handle
    if (!m_fs->m_session) return;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();

    // Check handle
    if (!m_fs->m_sftpSession) return;

    // Set opening flags
    unsigned long flags = LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT;
    if (mode & std::ios::app)   flags |= LIBSSH2_FXF_APPEND;
    if (mode & std::ios::trunc) flags |= LIBSSH2_FXF_TRUNC;

    // Open file
    m_file = (void *)libssh2_sftp_open((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str(), flags, 0);

    if (m_file)
    {
        // Initialize write buffer
        char * start = &m_buffer.front();
        char * end   = &m_buffer.front() + m_buffer.size();
        setp(start, end);
    }
}

SshOutputStreamBuffer::~SshOutputStreamBuffer()
{
    // Close file
    if (m_file)
    {
        // Flush buffer
        sync();

        // Sync file
        libssh2_sftp_fsync((LIBSSH2_SFTP_HANDLE *)m_file);
        libssh2_sftp_close((LIBSSH2_SFTP_HANDLE *)m_file);
    }
}

std::streambuf::int_type SshOutputStreamBuffer::overflow(std::streambuf::int_type value)
{
    // Check file handle
    if (!m_file)
    {
        return traits_type::eof();
    }

    // Sync buffer
    if (sync() != 0 || value == traits_type::eof())
    {
        return traits_type::eof();
    }

    // File in new value
    char * start = &m_buffer.front();
    char * end   = &m_buffer.front() + m_buffer.size();
    *start = value;

    // Update buffer position
    setp(start + 1, end);

    // Done
    return traits_type::to_int_type(value);
}

int SshOutputStreamBuffer::sync()
{
    // Get size of data in the buffer
    size_t size = pptr() - &m_buffer.front();
    if (size > 0)
    {
        // Write to stream
        auto res = libssh2_sftp_write((LIBSSH2_SFTP_HANDLE *)m_file, &m_buffer.front(), size);

        // Check for errors
        switch (res)
        {
            case LIBSSH2_ERROR_ALLOC:
                std::cout << "SSH error: LIBSSH2_ERROR_ALLOC" << std::endl;
                break;

            case LIBSSH2_ERROR_SOCKET_SEND:
                std::cout << "SSH error: LIBSSH2_ERROR_SOCKET_SEND" << std::endl;
                break;

            case LIBSSH2_ERROR_SOCKET_TIMEOUT:
                std::cout << "SSH error: LIBSSH2_ERROR_SOCKET_TIMEOUT" << std::endl;
                break;

            case LIBSSH2_ERROR_SFTP_PROTOCOL:
                std::cout << "SSH error: LIBSSH2_ERROR_SFTP_PROTOCOL" << std::endl;
                break;

            default:
                break;
        }

        // Reset write buffer
        char * start = &m_buffer.front();
        char * end   = &m_buffer.front() + m_buffer.size();
        setp(start, end);
    }

    // Done
    return 0;
}

SshOutputStreamBuffer::pos_type SshOutputStreamBuffer::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which)
{
    // Sync output first
    sync();

    if (way == std::ios_base::beg)
    {
        return seekpos((pos_type)off, which);
    }

    else if (way == std::ios_base::end)
    {
        LIBSSH2_SFTP_ATTRIBUTES attrs;

        if (libssh2_sftp_fstat_ex((LIBSSH2_SFTP_HANDLE *)m_file, &attrs, 0) == 0)
        {
            pos_type pos = (pos_type)attrs.filesize + off;
            return seekpos(pos, which);
        }
    }

    else if (way == std::ios_base::cur)
    {
        pos_type pos = (pos_type)libssh2_sftp_tell64((LIBSSH2_SFTP_HANDLE *)m_file);
        pos += off;

        return seekpos(pos, which);
    }

    return (pos_type)(off_type)(-1);
}

SshOutputStreamBuffer::pos_type SshOutputStreamBuffer::seekpos(pos_type pos, std::ios_base::openmode)
{
    // Sync output first
    sync();

    // Check file handle
    if (!m_file)
    {
        return (pos_type)(off_type)(-1);
    }

    // Set file position
    libssh2_sftp_seek64((LIBSSH2_SFTP_HANDLE *)m_file, (libssh2_uint64_t)pos);

    // Reset write buffer
    char * start = &m_buffer.front();
    char * end   = &m_buffer.front() + m_buffer.size();
    setp(start, end);

    // Return new position
    return (pos_type)(off_type)(pos);
}


} // namespace cppfs
