[//]: # (Comment)

# Cross-platform C++ file system library supporting multiple backends

*cppfs* is a [MIT licensed](http://opensource.org/licenses/MIT), cross-platform C++ library that
provides an object-oriented abstraction for working with files and the file system. It can be
used not only to access the local file system, but for remote and virtual file systems as well.
By specializing the virtual backend interface, *cppfs* can be easily extended to support additional
remote protocols or virtual file systems.

The following backends are currently implemented:
- Local file system (POSIX, win32)
- SSH (libssh2)


## Project Health

| Service | System | Compiler | Status |
| ------- | ------ | -------- | -----: |
|  [Travis-CI](https://travis-ci.org/cginternals/cppfs) | Ubuntu 14.04, macOS | GCC 4.8, Clang 3.5, AppleClang 7 | [![Build Status](https://travis-ci.org/cginternals/cppfs.svg?branch=master)](https://travis-ci.org/cginternals/cppfs) |
| [Coverity](https://scan.coverity.com/projects/6828?tab=overview) | Ubuntu 14.04 | GCC 5.3 | upcoming |
| Jenkins <br><br><br><br> | Ubuntu 14.04 <br><br><br><br> | GCC 4.8 <br> GCC 4.9 <br> GCC 5.4 <br> Clang 3.9 <br> | [![Build Status](https://jenkins.hpi3d.de/buildStatus/icon?job=cppfs-linux-gcc4.8)](https://jenkins.hpi3d.de/job/cppfs-linux-gcc4.8) <br> [![Build Status](https://jenkins.hpi3d.de/buildStatus/icon?job=cppfs-linux-gcc4.9)](https://jenkins.hpi3d.de/job/cppfs-linux-gcc4.9) <br> [![Build Status](https://jenkins.hpi3d.de/buildStatus/icon?job=cppfs-linux-gcc5)](https://jenkins.hpi3d.de/job/cppfs-linux-gcc5.4) <br> [![Build Status](https://jenkins.hpi3d.de/buildStatus/icon?job=cppfs-linux-clang3.9)](https://jenkins.hpi3d.de/job/cppfs-linux-clang3.9) <br> |
| Jenkins <br><br> | Windows 10 <br><br> | MSVC 2013 Update 5 <br>  MSVC 2015 Update 1 <br> | [![Build Status](https://jenkins.hpi3d.de/buildStatus/icon?job=cppfs-windows-msvc2013)](https://jenkins.hpi3d.de/job/cppfs-windows-msvc2013) <br> [![Build Status](https://jenkins.hpi3d.de/buildStatus/icon?job=cppfs-windows-msvc2015)](https://jenkins.hpi3d.de/job/cppfs-windows-msvc2015) <br> |

Please note that our macOS build node is currently broken (physically). However, *cppfs* is maintained for macOS as well and there are many people using it on macOS on a regular basis.


## Build Instructions

Please follow our [CMake project setup guide](https://github.com/cginternals/cmake-init/wiki/Setup-Guide)
to setup and build *cppfs*.


## Basic Example

Open a file for reading or writing:

```C++
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

void openFile(const std::string & filename)
{
    cppfs::FileHandle f = cppfs::fs::open(filename);

    if (in.isFile())
    {
        std::istream * in = f.createInputStream();
        ...

        std::istream * out = f.createOutputStream();
        ...
    }
}
```


## Features


### Paths and URLs

The class *FilePath* is used to represent paths in the file system.
It can be constructed from a string and converted back into a string.

```
cppfs::FilePath path("data/readme.txt");
std::string pathOut = path.path();
```

Paths are stored in a unified format using only forward slash ('/')
as a separator. The unified format is compatible to all systems,
but as a convenience for the user, it should be converted to the
native format when displaying paths to the user. To convert a path
into native format, call *toNative*.

```
cppfs::FilePath path("data/readme.txt");
std::cout << "File path: " << path.toNative() << std::endl;
```


### Filesystems

### File handles

### Advanced functions
