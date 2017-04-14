
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
#include <cppfs/Tree.h>
#include <cppfs/Diff.h>


using namespace cppassist;
using namespace cppfs;


int main(int argc, char * argv[])
{
    // Declare program
    CommandLineProgram program(
        "cppfs-sync",
        "cppfs-sync " CPPFS_VERSION,
        "Sync directories."
    );

    CommandLineAction action("sync", "Sync directories");
    program.add(&action);

    CommandLineSwitch swHelp("--help", "-h", "Print help text", CommandLineSwitch::Optional);
    action.add(&swHelp);

    CommandLineOption opConfig("--config", "-c", "file", "Load configuration from file", CommandLineOption::Optional);
    action.add(&opConfig);

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

    // Open directory handles
    FileHandle srcDir = fs::open(src, &login);
    FileHandle dstDir = fs::open(dst, &login);

    // Open source directory
    if (!srcDir.isDirectory())
    {
        // Error
        std::cout << "Could not open source directory '" << src << "'." << std::endl;
        return 1;
    }

    // Open destination directory
    if (!dstDir.isDirectory())
    {
        // Create destination directory
        dstDir.createDirectory();

        // Check directory again
        if (!dstDir.isDirectory())
        {
            // Error
            std::cout << "Could not open output directory '" << dst << "'." << std::endl;
            return 1;
        }
    }

    // Get both directory trees
    auto srcTree = srcDir.readTree();
    auto dstTree = dstDir.readTree();

    // Compute differences
    auto diff = dstTree->createDiff(*srcTree.get());

    // Apply changes to destination
    for (Change change : diff->changes())
    {
        if (change.operation() == Change::CopyFile) {
            FileHandle src = srcDir.open(change.path());
            FileHandle dst = dstDir.open(change.path());
            src.copy(dst);
        }

        if (change.operation() == Change::CopyDir) {
            FileHandle src = srcDir.open(change.path());
            FileHandle dst = dstDir.open(change.path());
            src.copyDirectoryRec(dst);
        }

        if (change.operation() == Change::RemoveFile) {
            FileHandle dst = dstDir.open(change.path());
            dst.remove();
        }

        if (change.operation() == Change::RemoveDir) {
            FileHandle dst = dstDir.open(change.path());
            dst.removeDirectoryRec();
        }
    }

    // Done
    return 0;
}
