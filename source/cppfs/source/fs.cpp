
#include <cppfs/fs.h>

#include <sstream>
#include <iomanip>
#include <iterator>

#include <basen/basen.hpp>

#if defined(__APPLE__)
    #define COMMON_DIGEST_FOR_OPENSSL
    #include <CommonCrypto/CommonDigest.h>
    #define SHA1 CC_SHA1
    #include <cppfs/ssh/SshFileSystem.h>
#elif defined(CPPFS_USE_OpenSSL)
    #include <openssl/sha.h>
    #include <cppfs/ssh/SshFileSystem.h>
#endif

#include <cppfs/system.h>
#include <cppfs/LoginCredentials.h>
#include <cppfs/Url.h>
#include <cppfs/FileHandle.h>
#include <cppfs/AbstractFileSystem.h>
#include <cppfs/FileIterator.h>

#ifdef SYSTEM_WINDOWS
    #include <cppfs/windows/LocalFileSystem.h>
#else
    #include <cppfs/posix/LocalFileSystem.h>
#endif


namespace cppfs
{
namespace fs
{


std::shared_ptr<AbstractFileSystem> localFS()
{
    static std::shared_ptr<LocalFileSystem> fs(new LocalFileSystem);

    return fs;
}

FileHandle open(const std::string & path, const LoginCredentials * credentials)
{
    // Parse url
    Url url(path);

    // Determine filesystem backend from path/url

    // SSH
    if (url.protocol() == "ssh://")
    {
#if defined(CPPFS_USE_OpenSSL)
        // Get connection parameters
        std::string host       = url.host();
        std::string user       = url.username();
        std::string pass       = url.password();
        std::string localPath  = url.path();
        int         port       = 22;
        std::string publicKey  = system::homeDir() + "/.ssh/id_rsa.pub";
        std::string privateKey = system::homeDir() + "/.ssh/id_rsa";

        // Apply login credentials
        if (credentials)
        {
            if (credentials->isSet("port"))       port = std::stoi(credentials->value("port"));
            if (credentials->isSet("username"))   user = credentials->value("username");
            if (credentials->isSet("password"))   pass = credentials->value("password");
            if (credentials->isSet("publicKey"))  publicKey = credentials->value("publicKey");
            if (credentials->isSet("privateKey")) privateKey = credentials->value("privateKey");
        }

        // Create SSH connection
        std::shared_ptr<SshFileSystem> fs(
            new SshFileSystem(host, port, user, pass, publicKey, privateKey)
        );

        // Open path
        return fs->open(localPath);
#else
        return FileHandle();
#endif
    }

    // Local file system
    else
    {
        // Get local path
        std::string localPath = url.path();

        // Open local file system
        auto fs = localFS();

        // Open path
        return fs->open(localPath);
    }
}

std::string sha1(const std::string & str)
{
#ifdef CPPFS_USE_OpenSSL
    // Initialize hash
    unsigned char hash[20];
    SHA_CTX context;
    SHA1_Init(&context);

    // Update hash
    SHA1_Update(&context, str.c_str(), str.size());

    // Compute hash
    SHA1_Final(hash, &context);
    return hashToString(hash);
#else
    return "";
#endif
}

std::string base64(const std::string & str)
{
    // Encode base64
    std::string base64;
    bn::encode_b64(str.begin(), str.end(), back_inserter(base64));

    // Return encoded string
    return base64;
}

std::string fromBase64(const std::string & base64)
{
    // Decode base64
    std::string str;
    bn::decode_b64(base64.begin(), base64.end(), back_inserter(str));

    // Return decoded string
    return str;
}

std::string hashToString(const unsigned char * hash)
{
    std::stringstream stream;
    stream << std::hex << std::setfill('0') << std::setw(2);

    for (int i=0; i<20; i++)
    {
        stream << static_cast<unsigned int>(hash[i]);
    }

    return stream.str();
}


} // namespace fs
} // namespace cppfs
