
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
*    Stream buffer for reading a SSH file
*/
class CPPFS_API SshInputStreamBuffer : public std::streambuf
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
    *    Size of the read buffer
    *  @param[n] putbackSize
    *    Size of the putback area
    */
    SshInputStreamBuffer(std::shared_ptr<SshFileSystem> fs, const std::string & path, std::ios_base::openmode mode, size_t bufferSize = size_kb(32), size_t putBackSize = size_b(128));

    /**
    *  @brief
    *    Destructor
    */
    virtual ~SshInputStreamBuffer();

    // Virtual streambuf functions
    virtual std::streambuf::int_type underflow() override;
    virtual pos_type seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
    virtual pos_type seekpos(pos_type pos, std::ios_base::openmode which) override;


protected:
    std::shared_ptr<SshFileSystem>   m_fs;          ///< File system that created this iterator
    const std::string                m_path;        ///< Path to file or directory
    void                           * m_file;        ///< SFTP file handle
    const size_t                     m_putbackSize; ///< Size of the putback area
    std::vector<char>                m_buffer;      ///< Read buffer
};


} // namespace cppfs
