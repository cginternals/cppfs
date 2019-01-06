
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

    CommandLineOption opConfig("--config", "-c", "file", "Load configuration from file", CommandLineOption::Optional);
    action.add(&opConfig);

    CommandLineOption opTime("--timeout", "-t", "seconds", "Timeout after which to stop (in seconds)", CommandLineOption::Optional);
    action.add(&opTime);

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

    // Execute file watching
    try {
        // Get path
        std::string path = paramPath.value();
        if (path.empty()) path = ".";

        // Open directory
        FileHandle dir = fs::open(path);
        if (dir.isDirectory())
        {
            // Watch directory
            FileWatcher watcher = dir.watch();

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
        }
        else
        {
            // Invalid path specified
            std::cout << "'" << path << "' is not a valid directory." << std::endl;
        }
    } catch (std::exception& e) {
        // Error during execution
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    // Done
    return 0;
}
