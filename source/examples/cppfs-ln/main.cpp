
#include <iostream>

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
        "cppfs-ln",
        "cppfs-ln " CPPFS_VERSION,
        "Create link."
    );

    CommandLineAction action("ln", "Create link");
    program.add(&action);

    CommandLineSwitch swHelp("--help", "-h", "Print help text", CommandLineSwitch::Optional);
    action.add(&swHelp);

    CommandLineOption opConfig("--config", "-c", "file", "Load configuration from file", CommandLineOption::Optional);
    action.add(&opConfig);

    CommandLineSwitch swSymbolic("--symbolic", "-s", "Create symbolic link", CommandLineSwitch::Optional);
    action.add(&swSymbolic);

    CommandLineParameter paramSrc("src", CommandLineParameter::NonOptional);
    action.add(&paramSrc);

    CommandLineParameter paramDst("dst", CommandLineParameter::NonOptional);
    action.add(&paramDst);

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

    // Get source and destination path
    std::string src = paramSrc.value();
    std::string dst = paramDst.value();

    // Open file handles
    FileHandle srcFile = fs::open(src, &login);
    FileHandle dstFile = fs::open(dst, &login);

    if (srcFile.isFile())
    {
        // Create link
        if (swSymbolic.activated()) srcFile.createSymbolicLink(dstFile);
        else                        srcFile.createLink(dstFile);

        // Done
        return 0;
    }

    else
    {
        // Error
        std::cout << "'" << src << "' is not a valid file." << std::endl;
    }

    return 0;
}
