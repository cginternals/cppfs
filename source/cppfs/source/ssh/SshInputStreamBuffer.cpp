
#include <cppfs/ssh/SshInputStreamBuffer.h>

#include <algorithm>
#include <cstring>
#include <iostream>

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <cppfs/ssh/SshFileSystem.h>


#ifdef max
    #undef max
#endif


namespace cppfs
{


SshInputStreamBuffer::SshInputStreamBuffer(std::shared_ptr<SshFileSystem> fs, const std::string & path, std::ios_base::openmode, size_t bufferSize, size_t putbackSize)
: m_fs(fs)
, m_path(path)
, m_file(nullptr)
, m_putbackSize(std::max(putbackSize, (size_t)1))
, m_buffer(std::max(bufferSize, m_putbackSize) + m_putbackSize)
{
    // Check handle
    if (!m_fs->m_session) return;

    // Initialize SFTP sub-protocol
    m_fs->initSftp();

    // Check handle
    if (!m_fs->m_sftpSession) return;

    // Open file
    m_file = (void *)libssh2_sftp_open((LIBSSH2_SFTP *)m_fs->m_sftpSession, m_path.c_str(), LIBSSH2_FXF_READ, 0);

    if (m_file)
    {
        // Initialize read buffer
        char * end = &m_buffer.front() + m_buffer.size();
        setg(end, end, end);
    }

    else
    {
        // Error, close file
        libssh2_sftp_close((LIBSSH2_SFTP_HANDLE *)m_file);
    }
}

SshInputStreamBuffer::~SshInputStreamBuffer()
{
    if (m_file)
    {
        libssh2_sftp_close((LIBSSH2_SFTP_HANDLE *)m_file);
    }
}

std::streambuf::int_type SshInputStreamBuffer::underflow()
{
    // Check file handle
    if (!m_file)
    {
        return traits_type::eof();
    }

    // Check if the buffer is filled
    if (gptr() < egptr())
    {
        // Return next byte from buffer
        return traits_type::to_int_type(*gptr());
    }

    // Prepare buffer
    char * base  = &m_buffer.front();
    char * start = base;

    if (eback() == base)
    {
        std::memmove(base, egptr() - m_putbackSize, m_putbackSize);
        start += m_putbackSize;
    }

    // Refill buffer
    size_t size = m_buffer.size() - (start - base);
    ssize_t n = libssh2_sftp_read((LIBSSH2_SFTP_HANDLE *)m_file, start, size);

    // EOF
    if (n == 0)
    {
        return traits_type::eof();
    }

    // Set buffer pointers
    setg(base, start, start + n);

    // Return next byte
    return traits_type::to_int_type(*gptr());
}

SshInputStreamBuffer::pos_type SshInputStreamBuffer::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which)
{
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
        pos += off - (egptr() - gptr());

        return seekpos(pos, which);
    }

    return (pos_type)(off_type)(-1);
}

SshInputStreamBuffer::pos_type SshInputStreamBuffer::seekpos(pos_type pos, std::ios_base::openmode)
{
    // Check file handle
    if (!m_file)
    {
        return (pos_type)(off_type)(-1);
    }

    // Set file position
    libssh2_sftp_seek64((LIBSSH2_SFTP_HANDLE *)m_file, (libssh2_uint64_t)pos);

    // Reset read buffer
    char * end = &m_buffer.front() + m_buffer.size();
    setg(end, end, end);

    // Return new position
    return (pos_type)(off_type)(pos);
}


} // namespace cppfs
