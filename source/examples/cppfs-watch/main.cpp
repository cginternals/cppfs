
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

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

    CommandLineOption opTime("--timeout", "-t", "seconds", "Stop watch loop after given time", CommandLineOption::Optional);
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
                while (true) {
                    watcher.watch();
                }
            }
            else {
                auto t_sleep = std::stoi(t);
                std::cout << "Will stop watching after " << t_sleep << " seconds..." << std::endl;

                // Execute watch loop in a separate thread
                std::atomic<bool> stop_thread{false};
                std::thread thrd([&] {
                    while (!stop_thread) {
                        watcher.watch(50);  // Timeout 50 ms, so we can poll stop_thread variable
                    }
                });

                // Zzzzz....
                std::this_thread::sleep_for(std::chrono::seconds(t_sleep));

                // Join watcher thread
                stop_thread = true;
                thrd.join();
            }
        }
        else
        {
            std::cout << "'" << path << "' is not a valid directory." << std::endl;
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }

    // Done
    return 0;
}
