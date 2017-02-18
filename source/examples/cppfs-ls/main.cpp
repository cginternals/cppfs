
#include <iostream>

#include <cppassist/cmdline/CommandLineProgram.h>
#include <cppassist/cmdline/CommandLineAction.h>
#include <cppassist/cmdline/CommandLineSwitch.h>
#include <cppassist/cmdline/CommandLineParameter.h>

#include <cppfs/cppfs-version.h>
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>


using namespace cppassist;
using namespace cppfs;


void printInfo(const FileHandle & fh)
{
    std::cout << "File '" << fh.path() << "' ";

    if (fh.exists())
    {
        std::cout << (fh.isDirectory() ? "is a directory" : "is a file") << std::endl;
    }
    else
    {
        std::cout << "does not exist" << std::endl;
    }
}


int main(int argc, char * argv[])
{
    // Declare program
    CommandLineProgram program(
        "cppfs-ls",
        "cppfs-ls " CPPFS_VERSION,
        "List all files in a directory."
    );

    CommandLineAction action("list", "List files in directory");
    program.add(&action);

    CommandLineSwitch swHelp("--help", "-h", "Print help text", CommandLineSwitch::Optional);
    action.add(&swHelp);

    CommandLineParameter paramPath("path", CommandLineParameter::Optional);
    action.add(&paramPath);

    // Parse command line
    program.parse(argc, argv);
    if (program.hasErrors() || swHelp.activated())
    {
        // Print help and exit
        program.print(program.help());
        return 0;
    }

    // Get path
    std::string path = paramPath.value();
    if (path.empty()) path = ".";

    // Open directory
    FileHandle dir = fs::open(path);

    if (dir.isDirectory())
    {
        // List files
        for (auto it = dir.begin(); it != dir.end(); ++it)
        {
            std::cout << "- " << *it << std::endl;
        }
    }

    else
    {
        std::cout << "'" << path << "' is not a valid directory." << std::endl;
    }

    // Done
    return 0;
}
