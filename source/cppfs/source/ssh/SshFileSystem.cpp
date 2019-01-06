
#include <cppfs/ssh/SshFileSystem.h>

#ifdef WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
#endif

#include <libssh2.h>
#include <libssh2_sftp.h>

#include <cppfs/FileHandle.h>
#include <cppfs/AbstractFileWatcherBackend.h>
#include <cppfs/ssh/SshFileHandle.h>


namespace cppfs
{


SshFileSystem::SshFileSystem(const std::string & host, int port, const std::string & username, const std::string & password, const std::string & publicKey, const std::string & privateKey)
: m_host(host)
, m_port(port)
, m_username(username)
, m_password(password)
, m_publicKey(publicKey)
, m_privateKey(privateKey)
, m_socket(0)
, m_session(nullptr)
, m_sftpSession(nullptr)
{
    connect();
}

SshFileSystem::SshFileSystem(std::string && host, int port, std::string && username, std::string && password, std::string && publicKey, std::string && privateKey)
: m_host(std::move(host))
, m_port(std::move(port))
, m_username(std::move(username))
, m_password(std::move(password))
, m_publicKey(std::move(publicKey))
, m_privateKey(std::move(privateKey))
, m_socket(0)
, m_session(nullptr)
, m_sftpSession(nullptr)
{
    connect();
}

SshFileSystem::~SshFileSystem()
{
    disconnect();
}

FileHandle SshFileSystem::open(const std::string & path)
{
    return open(std::string(path));
}

FileHandle SshFileSystem::open(std::string && path)
{
    return FileHandle(
        std::unique_ptr<AbstractFileHandleBackend>(
            new SshFileHandle(shared_from_this(), path)
        )
    );
}

std::unique_ptr<AbstractFileWatcherBackend> SshFileSystem::createFileWatcher(FileWatcher & fileWatcher)
{
    return nullptr;
}

void SshFileSystem::connect()
{
#ifdef WIN32
    // Initialize winsock
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,0), &wsadata);
#endif

    // Close connection if it is already open
    disconnect();

    // Initialize libssh2
    libssh2_init(0);

    // Lookup host name
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo * addrInfo = nullptr;
    if ((getaddrinfo(m_host.c_str(), std::to_string(m_port).c_str(), &hints, &addrInfo)) != 0)
    {
        // Error!
        return;
    }

    // Try to connect
    m_socket = -1;
    for (struct addrinfo * address = addrInfo; address != nullptr; address = address->ai_next)
    {
        // Create socket
        if ((m_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol)) == -1)
        {
            // Error!
            continue;
        }

        // Connect
        if (::connect(m_socket, address->ai_addr, address->ai_addrlen) == -1)
        {
            // Error!
            #ifdef WIN32
                _close(m_socket);
            #else
                close(m_socket);
            #endif
            continue;
        }
    }

    // Clean up
    freeaddrinfo(addrInfo);

    // Check if connection has been successfull
    if (m_socket == -1)
    {
        return;
    }

    // Create libssh2 handle
    m_session = libssh2_session_init();
    if (m_session)
    {
        // Open session
        int res = libssh2_session_handshake((LIBSSH2_SESSION *)m_session, m_socket);
        /*
        if (res == LIBSSH2_ERROR_SOCKET_NONE)       std::cout << "LIBSSH2_ERROR_SOCKET_NONE" << std::endl;
        if (res == LIBSSH2_ERROR_BANNER_SEND)       std::cout << "LIBSSH2_ERROR_BANNER_SEND" << std::endl;
        if (res == LIBSSH2_ERROR_KEX_FAILURE)       std::cout << "LIBSSH2_ERROR_KEX_FAILURE" << std::endl;
        if (res == LIBSSH2_ERROR_SOCKET_SEND)       std::cout << "LIBSSH2_ERROR_SOCKET_SEND" << std::endl;
        if (res == LIBSSH2_ERROR_SOCKET_DISCONNECT) std::cout << "LIBSSH2_ERROR_SOCKET_DISCONNECT" << std::endl;
        if (res == LIBSSH2_ERROR_PROTO)             std::cout << "LIBSSH2_ERROR_PROTO" << std::endl;
        if (res == LIBSSH2_ERROR_EAGAIN)            std::cout << "LIBSSH2_ERROR_EAGAIN" << std::endl;
        */

        if (res == 0)
        {
            // Get fingerprint
            std::string fingerprint(libssh2_hostkey_hash((LIBSSH2_SESSION *)m_session, LIBSSH2_HOSTKEY_HASH_SHA1));

            // Try public key authentication first
            int connect = libssh2_userauth_publickey_fromfile(
                (LIBSSH2_SESSION *)m_session, m_username.c_str(),
                m_publicKey.c_str(),
                m_privateKey.c_str(),
                m_password.c_str()
            );

            // Try username/password second
            if (connect != 0)
            {
                connect = libssh2_userauth_password((LIBSSH2_SESSION *)m_session, m_username.c_str(), m_password.c_str());
            }

            // Have we connected successfully?
            if (connect == 0)
            {
                // Set SSH mode to blocking
                libssh2_session_set_blocking((LIBSSH2_SESSION *)m_session, 1);
            }
            else
            {
                // ERROR, could not connect
                disconnect();
            }
        }
    }
}

void SshFileSystem::disconnect()
{
    // Check if session is already closed
    if (!m_session) return;

    // Close SFTP session
    if (m_sftpSession)
    {
        libssh2_sftp_shutdown((LIBSSH2_SFTP *)m_sftpSession);
    }

    // Close SSH session
    libssh2_session_disconnect((LIBSSH2_SESSION *)m_session, "done");
    libssh2_session_free      ((LIBSSH2_SESSION *)m_session);

    // Close socket
    if (m_socket != 0)
    {
        #ifdef WIN32
            closesocket(m_socket);
        #else
            ::close(m_socket);
        #endif
    }

    // Reset state
    m_socket      = 0;
    m_session     = nullptr;
    m_sftpSession = nullptr;
}

void SshFileSystem::initSftp()
{
    // Check if SFTP subprotocol has already been initialize
    if (m_sftpSession) return;

    // Open SFTP session
    m_sftpSession = libssh2_sftp_init((LIBSSH2_SESSION *)m_session);
}

void SshFileSystem::checkError()
{
    // Get SSH error message
    char * errorMsg;
    int    len = 1024;
    libssh2_session_last_error((LIBSSH2_SESSION *)m_session, &errorMsg, &len, 0);

    // [TODO] Log error
//  std::cout << "SSH ERROR: " << errorMsg << "\n";

    // To recover from errors, it seems to be necessary to reconnect :(
    connect();
}


} // namespace cppfs
