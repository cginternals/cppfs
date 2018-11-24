
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
#include <cppfs/FileIterator.h>
#include <cppfs/FileWatcher.h>


using namespace cppassist;
using namespace cppfs;


int main(int argc, char * argv[])
{
    // Declare program
    CommandLineProgram program(
        "cppfs-watch",
        "cppfs-watch " CPPFS_VERSION,
        "Listen to events in the file system and print them to the console."
    );

    CommandLineAction action("watch", "Watch files in directory");
    program.add(&action);

    CommandLineSwitch swHelp("--help", "-h", "Print help text", CommandLineSwitch::Optional);
    action.add(&swHelp);

    CommandLineOption opConfig("--config", "-c", "file", "Load configuration from file", CommandLineOption::Optional);
    action.add(&opConfig);

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

    // Get login credentials
    LoginCredentials login;

    std::string configFile = opConfig.value();
    if (!configFile.empty())
    {
        login.load(configFile);
    }

    // Get path
    std::string path = paramPath.value();
    if (path.empty()) path = ".";

    // Create file system watcher
    FileWatcher watcher;

    // Open directory
    FileHandle dir = fs::open(path, &login);
    if (dir.isDirectory())
    {
        // Watch directory
        watcher.add(dir);

        // Create file event handler
        watcher.addHandler([] (FileHandle & fh, FileEvent event) {
            // Get file type
            std::string type = (fh.isDirectory() ? "directory" : "file");

            // Get operation
            std::string operation = ( (event & FileCreated) ? "created" :
                                    ( (event & FileRemoved) ? "removed" :
                                                              "modified" ) );

            // Log event
            std::cout << "The " << type << " '" << fh.path() << "' was " << operation << "." << std::endl;
        });

        // Begin watching and printing events
        while (true) {
            watcher.watch();
        }
    }
    else
    {
        std::cout << "'" << path << "' is not a valid directory." << std::endl;
    }

    // Done
    return 0;
}
