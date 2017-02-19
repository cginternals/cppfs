
#include <iostream>
#include <sstream>

#include <cppassist/cmdline/CommandLineProgram.h>
#include <cppassist/cmdline/CommandLineAction.h>
#include <cppassist/cmdline/CommandLineSwitch.h>
#include <cppassist/cmdline/CommandLineOption.h>
#include <cppassist/cmdline/CommandLineParameter.h>

#include <cppfs/cppfs-version.h>
#include <cppfs/fs.h>
#include <cppfs/LoginCredentials.h>
#include <cppfs/FileHandle.h>


using namespace cppassist;
using namespace cppfs;


int main(int argc, char * argv[])
{
    // Declare program
    CommandLineProgram program(
        "cppfs-cat",
        "cppfs-cat " CPPFS_VERSION,
        "Print file to console."
    );

    CommandLineAction action("cat", "Print file to console");
    program.add(&action);

    CommandLineSwitch swHelp("--help", "-h", "Print help text", CommandLineSwitch::Optional);
    action.add(&swHelp);

    CommandLineOption opConfig("--config", "-c", "file", "Load configuration from file", CommandLineOption::Optional);
    action.add(&opConfig);

    CommandLineParameter paramPath("path", CommandLineParameter::NonOptional);
    action.add(&paramPath);

    // Parse command line
    program.parse(argc, argv);
    if (program.hasErrors() || swHelp.activated())
    {
        // Print help and exit
        program.print(program.help());
        return 0;
    }

    // Get login credentials
    LoginCredentials login;

    std::string configFile = opConfig.value();
    if (!configFile.empty())
    {
        login.load(configFile);
    }

    // Get path
    std::string path = paramPath.value();

    // Open file
    FileHandle file = fs::open(path, &login);
    if (file.isFile())
    {
        // Open input stream
        std::istream * inputStream = file.createInputStream();
        if (inputStream)
        {
            // Read entire file
            std::stringstream buffer;
            buffer << inputStream->rdbuf();

            // Output content
            std::cout << buffer.str() << std::endl;

            // Destroy input stream
            delete inputStream;

            // Done
            return 0;
        }
    }

    // Error
    std::cout << "'" << path << "' is not a valid file." << std::endl;
    return 0;
}
