
#pragma once


#include <string>
#include <memory>
#include <vector>
#include <streambuf>

#include <cppfs/cppfs_api.h>
#include <cppfs/units.h>


namespace cppfs
{


class SshFileSystem;


/**
*  @brief
*    Stream buffer for writing a SSH file
*/
class CPPFS_API SshOutputStreamBuffer : public std::streambuf
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] fs
    *    File system that created this iterator
    *  @param[in] path
    *    Path to file or directory
    *  @param[in] mode
    *    Opening mode flags
    *  @param[n] bufferSize
    *    Size of the internal buffer
    *
    *  @remarks
    *    Do not set bufferSize to higher values, as libssh2 does not seem to
    *    support it. Testing revealed any values heigher than 24k to fail
    *    (files ended up not being written entirely).
    *
    *    See https://c-ares.haxx.se/mail/libssh2-devel-archive-2010-02/0170.shtml
    */
    SshOutputStreamBuffer(std::shared_ptr<SshFileSystem> fs, const std::string & path, std::ios_base::openmode mode, size_t bufferSize = size_kb(24));

    /**
    *  @brief
    *    Destructor
    */
    virtual ~SshOutputStreamBuffer();

    // Virtual streambuf functions
    virtual std::streambuf::int_type overflow(std::streambuf::int_type value) override;
    virtual int sync() override;
    virtual pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;


protected:
    std::shared_ptr<SshFileSystem>         m_fs;     ///< File system that created this iterator
    const std::string                      m_path;   ///< Path to file or directory
    void                                 * m_file;   ///< SFTP file handle
    std::vector<std::streambuf::char_type> m_buffer; ///< Write buffer
};


} // namespace cppfs
