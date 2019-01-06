
#include <chrono>
#include <iostream>

#include <cppassist/cmdline/CommandLineAction.h>
#include <cppassist/cmdline/CommandLineOption.h>
#include <cppassist/cmdline/CommandLineParameter.h>
#include <cppassist/cmdline/CommandLineProgram.h>
#include <cppassist/cmdline/CommandLineSwitch.h>

#include <cppfs/FileHandle.h>
#include <cppfs/FileIterator.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/LoginCredentials.h>
#include <cppfs/cppfs-version.h>
#include <cppfs/fs.h>


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

    CommandLineSwitch swRecursive("--recursive", "-r", "Watcher directories recursively", CommandLineSwitch::Optional);
    action.add(&swRecursive);

    CommandLineOption opTime("--timeout", "-t", "seconds", "Timeout after which to stop (in seconds)", CommandLineOption::Optional);
    action.add(&opTime);

    action.setOptionalParametersAllowed(true);
    action.setOptionalParameterName("path");

    // Parse command line
    program.parse(argc, argv);
    if (program.hasErrors() || swHelp.activated())
    {
        // Print help and exit
        program.print(program.help());
        return 0;
    }

    // Execute file watching
    try {
        // Create file watcher
        FileWatcher watcher;

        // Get recursive mode
        RecursiveMode recursive = swRecursive.activated() ? Recursive : NonRecursive;

        // Get paths to watch
        auto paths = action.optionalParameters();
        if (paths.size() < 1) {
            paths.push_back(".");
        }

        // Add directories to watcher
        for (auto path : paths)
        {
            // Open directory
            FileHandle dir = fs::open(path);
            if (dir.isDirectory())
            {
                std::cout << "Watching '" << path << "'" << std::endl;

                // Watch directory
                watcher.add(dir, FileCreated | FileRemoved | FileModified | FileAttrChanged, recursive);
            }
            else
            {
                // Invalid directory
                std::cout << "'" << path << "' is not a valid directory." << std::endl;
            }
        }

        // Create file event handler
        watcher.addHandler([] (FileHandle & fh, FileEvent event) {
            // Get file type
            std::string type = (fh.isDirectory() ? "directory" : "file");

            // Get operation
            std::string operation = ( (event & FileCreated)     ? "created" :
                                    ( (event & FileRemoved)     ? "removed" :
                                    ( (event & FileAttrChanged) ? "attributes changed" :
                                                                  "modified" ) ) );

            // Log event
            std::cout << "The " << type << " '" << fh.path() << "' was " << operation << "." << std::endl;
        });

        // Begin watching and printing events
        std::string t = opTime.value();
        if (t.empty()) {
            // No timeout given
            while (true) {
                watcher.watch();
            }
        }
        else {
            // Get timeout
            auto timeout = std::stoi(t);
            std::cout << "Will stop watching after " << timeout << " seconds..." << std::endl;

            // Get current time
            auto start = std::chrono::system_clock::now();
            auto now   = std::chrono::system_clock::now();

            // Execute watch loop
            while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count() < timeout)
            {
                // Timeout 500ms to revive the main loop
                watcher.watch(500);

                // Check elapsed time
                now = std::chrono::system_clock::now();
            }
        }
    } catch (std::exception& e) {
        // Error during execution
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    // Done
    return 0;
}
