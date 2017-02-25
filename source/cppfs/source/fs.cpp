
#include <cppfs/fs.h>

#include <sstream>
#include <iomanip>

#if defined(__APPLE__)
    #define COMMON_DIGEST_FOR_OPENSSL
    #include <CommonCrypto/CommonDigest.h>
    #define SHA1 CC_SHA1
#else
    #include <openssl/sha.h>
#endif

#include <cppassist/string/conversion.h>

#include <cppfs/system.h>
#include <cppfs/LoginCredentials.h>
#include <cppfs/Url.h>
#include <cppfs/FileHandle.h>
#include <cppfs/ssh/SshFileSystem.h>

#ifdef SYSTEM_WINDOWS
    #include <cppfs/windows/LocalFileSystem.h>
#else
    #include <cppfs/posix/LocalFileSystem.h>
#endif


using namespace cppassist;


namespace
{


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


}


namespace cppfs
{
namespace fs
{


FileHandle open(const std::string & path, const LoginCredentials * credentials)
{
    // Parse url
    Url url(path);

    // Determine filesystem backend from path/url

    // SSH
    if (url.protocol() == "ssh://")
    {
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
            if (credentials->isSet("port"))       port = string::fromString<int>(credentials->value("port"));
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
    }

    // Local file system
    else
    {
        // Get local path
        std::string localPath = url.path();

        // Open local file system
        static std::shared_ptr<LocalFileSystem> fs(new LocalFileSystem);

        // Open path
        return fs->open(localPath);
    }
}

std::string readFile(const std::string & path)
{
    FileHandle fh = open(path);

    if (fh.isFile())
    {
        std::istream * inputStream = fh.createInputStream();
        if (!inputStream) return "";

        std::stringstream buffer;
        buffer << inputStream->rdbuf();

        delete inputStream;

        return buffer.str();
    }

    return "";
}

std::string sha1(const std::string & str)
{
    // Initialize hash
    unsigned char hash[20];
    SHA_CTX context;
    SHA1_Init(&context);

    // Update hash
    SHA1_Update(&context, str.c_str(), str.size());

    // Compute hash
    SHA1_Final(hash, &context);
    return hashToString(hash);
}

std::string sha1(const FileHandle & file)
{
    // Check file
    if (!file.isFile()) {
        return "";
    }

    // Open file
    std::istream * inputStream = file.createInputStream();
    if (!inputStream) {
        return "";
    }

    // Initialize hash
    unsigned char hash[20];
    SHA_CTX context;
    SHA1_Init(&context);

    // Read whole while
    while (!inputStream->eof())
    {
        // Read a maximum of 1024 bytes at once
        int size = 1024;

        // Read data from file
        char buf[1024];
        inputStream->read(buf, size);

        size_t count = inputStream->gcount();
        if (count > 0)
        {
            // Update hash
            SHA1_Update(&context, buf, count);
        } else break;
    }

    // Close file
    delete inputStream;

    // Compute hash
    SHA1_Final(hash, &context);
    return hashToString(hash);
}


} // namespace fs
} // namespace cppfs
