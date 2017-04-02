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

```C++
cppfs::FilePath path("data/readme.txt");
std::string pathOut = path.path();
```

Paths are stored in a unified format using only forward slashes ('/')
as a separator. The unified format is compatible to all systems,
but as a convenience for the user, it should be converted to the
native format when displaying paths. To convert a path into the native
format, call *toNative*.

```C++
cppfs::FilePath path("data/readme.txt");
std::cout << "File path: " << path.toNative() << std::endl;
```

A *FilePath* can be used to obtain syntactical information about a path.
It does however work purely on the string, it cannot provide information
about the actual file or directory the path points to. The following
functions are useful to get information about a path:

```C++
cppfs::FilePath path = ...;

// Check if the path is empty ("")
bool empty = path.isEmpty();

// Check if the path is an absolute path (e.g., "/test.txt", or "C:/test.txt")
bool abs = path.isAbsolute();

// Check if the path is a relative path (e.g., "data/test.txt")
bool rel = path.isRelative();

// Check if path points to a file/directory (e.g., "/path/to/dir")
// or its content (e.g., "/path/to/dir/").
bool listContent = path.pointsToContent();
```

*FilePath* offers functions to obtain the individual components of a file path,
such as the filename, extension, or path to the containing directory.
All of these functions ignore trailing slashes on the path, so they work on
the object the path points to, not it contents.

```C++
cppfs::FilePath path1("C:/path/to/file.txt");
cppfs::FilePath path2("C:/path/to/directory/");

// Get full path
std::cout << path1.fullPath() << std::endl; // "C:/path/to/file.txt"
std::cout << path2.fullPath() << std::endl; // "C:/path/to/directory"

// Get filename components
std::cout << path1.fileName()  << std::endl; // "file.txt"
std::cout << path2.fileName()  << std::endl; // "directory"
std::cout << path1.baseName()  << std::endl; // "file"
std::cout << path2.baseName()  << std::endl; // "directory"
std::cout << path1.extension() << std::endl; // ".txt"
std::cout << path2.extension() << std::endl; // ""

// Get path to containing directory
std::cout << path1.directoryPath() << std::endl; // "C:/path/to/"
std::cout << path2.directoryPath() << std::endl; // "C:/path/to/"

// Get drive letter
std::cout << path1.driveLetter() << std::endl; // "C:"
std::cout << path2.driveLetter() << std::endl; // "C:"
```

Paths often need to be combined in order to determine the actual location
of a file. To combine two paths, the function *resolve* can be used. The
second path must be relative to combine the two paths, otherwise only the
second path is returned. The combination of paths also takes place on
a pure syntactical level, without checking if any of the paths really exist.

```C++
cppfs::FilePath src("C:/projects");
cppfs::FilePath rel("../documents/test.txt");
cppfs::FilePath abs("C:/projects2/readme.txt");

cppfs::FilePath path1 = src.resolve(rel);
std::cout << path1.fullPath() << std::endl; // "C:/projects/../documents/test.txt"

cppfs::FilePath path2 = src.resolve(abs);
std::cout << path2.fullPath() << std::endl; // "C:/projects2/readme.txt"
```

When combining relative paths, the resulting strings may contain a lot
of ".." and "." components. To resolve these on a syntactical level, the
function *resolved* can be called. It will remove all occurences of "."
and "..", as long as it is possible. Occurences of ".." at the beginning
of a path will not be removed.

```C++
cppfs::FilePath path("C:/projects/../documents/test.txt");
std::cout << path.resolved() << std::endl; // "C:/documents/test.txt"
```


### Accessing the file system

The main class for accessing file and directories in cppfs is *FileHandle*.
It can be used to get information about file system objects, to manipulate
them (e.g., copy, rename, or delete), as well as to read and write files.

To obtain a file handle, the global function *fs::open* can be used. The type of file
system that is accessed will be determined automatically by the given path
or URL. File systems will be closed automatically when they no longer have
any open file handles.

```C++
using namespace cppfs;

// Open local file
FileHandle file1 = fs::open("data/readme.txt");

// Open file on SSH server
LoginCredentials login;
login.setValue("username",   "username");
login.setValue("password",   "password");
login.setValue("publicKey",  "/path/to/key.pub"); // Default: "$home/.ssh/id_rsa.pub"
login.setValue("privateKey", "/path/to/key");     // Default: "$home/.ssh/id_rsa"
login.setValue("port",       "999");              // Default: 22

FileHandle file2 = fs::open("ssh://example.com/home/user/readme.txt", &login);
```

At the moment, it is not possible to register new file systems at the global
level. To use a custom file system, create an instance of it and use the
*cppfs::AbstractFileSystem* interface to access it.

```C++
class CustomFS : public cppfs::AbstractFileSystem
{
    ...
};

CustomFS fs;
FileHandle file = fs.open("data/readme.txt");
```


### Getting file information

A file handle can be used to access information about the file
system object it points to. A file handle can also be copied,
creating a second handle that points to the same file system
object but does not inherit the state of the former handle.
This operation only copies the path of the handle to the
new object, so it is a cheap operation.

```C++
// Open file from local file system
cppfs::FileHandle file = cppfs::fs::open("data/readme.txt");

// Get a second handle to the same file
cppfs::FileHandle file2 = file;
```

Once a file handle has been obtained, it can be used to query basic information
about the file system object is points to.

```C++
cppfs::FileHandle file = cppfs::fs::open("data/readme.txt");

// Check type
     if (file.isFile())         std::cout << "file" << std::endl;
else if (file.isDirectory())    std::cout << "directory" << std::endl;
else if (file.isSymbolicLink()) std::cout << "symlink" << std::endl;
else if (!file.exists())        std::cout << "file does not exist" << std::endl;

// Get filename and path
std::string path = file.path();
std::string filename = file.fileName();

// Get file information
unsigned int  size  = file.size();
unsigned int  atime = file.accessTime();
unsigned int  mtime = file.modificationTime();
unsigned int  uid   = file.userId();
unsigned int  gid   = file.groupId();
unsigned long perm  = file.permissions();
```

File permissions can also be modified:

```C++
cppfs::FileHandle file = cppfs::fs::open("data/readme.txt");

file.setUserId(1000);
file.setGroupId(1000);
file.setPermissions(cppfs::FilePermissions::UserRead | cppfs::FilePermissions::GroupRead);
```

The file information is retrieved only when needed, i.e., on the first call
of one of the above functions, and then cached in memory. When an operation on
the handle modifies the file information, it is updated automatically.
However, if a file is modified outside of the application or using a different
handle to the same file, the file handle cannot know about the change. Therefore,
it must be updated manually:

```C++
cppfs::FileHandle file = cppfs::fs::open("data/readme.txt");

// If the file has been modified outside the application ...
file.updateFileInfo();
```


### Accessing and traversing directories

A *FileHandle* is used to access files as well as directories.
To check if a file handle points to a directory, the function
*isDirectory* can be used.

```C++
cppfs::FileHandle dir = cppfs::fs::open("data");
if (dir.isDirectory())
{
    ...
}
```

To list all files in a directory, call *listFiles*:

```C++
std::vector<std::string> files = dir.listFiles();
```

For better control, the C++ iterator interface can be used:

```C++
for (FileIterator it = dir.begin(); it != dir.end(); ++it)
{
    std::string path = *it;
}
```

For automatically traversing a directory tree, the *traverse*
function can be called. It can be passed  either
- a callback function for each file entry
- a callback function for each file and one for each directory
- a visitor object

```C++
// Traverse all file entries
dir.traverse([](FileHandle & fh) -> bool {
    std::cout << fh.path() << std::endl;
    return true; // continue
});

// Traverse all files and directories
dir.traverse([](FileHandle & fh) -> bool {
    std::cout << fh.path() << std::endl;
    return true; // continue
}, [](FileHandle & dir) -> bool {
    std::cout << dir.path() << std::endl;
    return true; // continue
});
```

Once ...

FileHandle parentDirectory() const;
FileHandle open(const std::string & path) const;


### File trees

Tree * readTree(const std::string & path = "", bool includeHash = false) const;
Diff
Change


### File operations

bool createDirectory();
bool removeDirectory();
bool copy(FileHandle & dest);
bool move(FileHandle & dest);
bool createLink(FileHandle & dest);
bool createSymbolicLink(FileHandle & dest);
bool rename(const std::string & filename);
bool remove();


### Reading and writing into files

std::istream * createInputStream(std::ios_base::openmode mode = std::ios_base::in) const;
std::ostream * createOutputStream(std::ios_base::openmode mode = std::ios_base::out);

std::string readFile() const;
bool writeFile(const std::string & content);


### Advanced functions

std::string sha1() const;
std::string base64() const;
bool writeFileBase64(const std::string & base64);
