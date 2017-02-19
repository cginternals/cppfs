
#include <cppfs/fs.h>

#include <sstream>

#include <cppassist/string/conversion.h>

#include <cppfs/system.h>
#include <cppfs/LoginCredentials.h>
#include <cppfs/Url.h>
#include <cppfs/FileHandle.h>
#include <cppfs/ssh/SshFileSystem.h>

#ifdef SYSTEM_WINDOWS
	#include <cppfs/windows/WinFileSystem.h>
#else
	#include <cppfs/local/LocalFileSystem.h>
#endif


using namespace cppassist;


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
#ifdef SYSTEM_WINDOWS
		static std::shared_ptr<WinFileSystem> fs(new WinFileSystem);
#else
		static std::shared_ptr<LocalFileSystem> fs(new LocalFileSystem);
#endif

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


} // namespace fs
} // namespace cppfs
