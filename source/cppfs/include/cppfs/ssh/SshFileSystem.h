
#pragma once


#include <memory>
#include <string>

#include <cppfs/AbstractFileSystem.h>


namespace cppfs
{


/**
*  @brief
*    Representation of a remote file system accessed by SSH
*/
class CPPFS_API SshFileSystem : public AbstractFileSystem, public std::enable_shared_from_this<SshFileSystem>
{
    friend class SshFileHandle;
    friend class SshFileIterator;
    friend class SshInputStreamBuffer;
    friend class SshOutputStreamBuffer;


public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] host
    *    Host name
    *  @param[in] port
    *    Port
    *  @param[in] username
    *    User name
    *  @param[in] password
    *    Password
    *  @param[in] publicKey
    *    Path to public key file
    *  @param[in] privateKey
    *    Path to private key file
    */
    SshFileSystem(
        const std::string & host,
        int port,
        const std::string & username,
        const std::string & password,
        const std::string & publicKey,
        const std::string & privateKey
    );

    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] host
    *    Host name
    *  @param[in] port
    *    Port
    *  @param[in] username
    *    User name
    *  @param[in] password
    *    Password
    *  @param[in] publicKey
    *    Path to public key file
    *  @param[in] privateKey
    *    Path to private key file
    */
    SshFileSystem(
        std::string && host,
        int port,
        std::string && username,
        std::string && password,
        std::string && publicKey,
        std::string && privateKey
    );

    /**
    *  @brief
    *    Destructor
    */
    virtual ~SshFileSystem();

    // Virtual AbstractFileSystem functions
    virtual FileHandle open(const std::string & path) override;
    virtual FileHandle open(std::string && path) override;
    virtual std::unique_ptr<AbstractFileWatcherBackend> createFileWatcher(FileWatcher & fileWatcher) override;


protected:
    void connect();
    void disconnect();
    void initSftp();
    void checkError();


protected:
    // Configuration
    std::string m_host;
    int         m_port;
    std::string m_username;
    std::string m_password;
    std::string m_publicKey;
    std::string m_privateKey;

    // Connection
    int    m_socket;      ///< Socket to host
    void * m_session;     ///< SSH session handle
    void * m_sftpSession; ///< SFTP session handle
};


} // namespace cppfs
