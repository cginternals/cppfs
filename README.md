[//]: # (Comment)

<br><a href="https://github.com/cginternals/cppfs/"><img src="https://raw.githubusercontent.com/cginternals/cppfs/master/cppfs-logo.svg?sanitize=true" width="50%"></a>

*cppfs* is a cross-platform C++ library that provides an object-oriented abstraction for working with files and the file system. 

![GitHub release](https://img.shields.io/github/release/cginternals/cppfs.svg) 
[![Travis](https://img.shields.io/travis/cginternals/cppfs/master.svg?style=flat&logo=travis)](https://travis-ci.org/cginternals/cppfs)
[![Appveyor](https://img.shields.io/appveyor/ci/scheibel/cppfs/master.svg?style=flat&logo=appveyor)](https://ci.appveyor.com/project/scheibel/cppfs/branch/master)
[![Tokei](https://tokei.rs/b1/github/cginternals/cppfs)](https://github.com/Aaronepower/tokei)
[![Tokei](https://tokei.rs/b1/github/cginternals/cppfs?category=comments)](https://github.com/Aaronepower/tokei)

*cppfs* can be used not only to access the local file system, but for remote and virtual file systems as well.
By specializing the virtual backend interface, *cppfs* can be easily extended to support additional
remote protocols or virtual file systems.
The following backends are currently implemented:
- Local file system (POSIX, win32)
- SSH (libssh2)


# Resources

* [Basic Examples](#basic-examples)

##### Installation and Development

* [Build form Source](#build-instructions)
* [Tips for Linking](#tips-for-linking)

##### Features
 * [Paths and URLs](#paths-and-urls)
 * [Accessing the file system](#accessing-the-file-system)
 * [Getting file information](#getting-file-information)
 * [File operations](#file-operations)
 * [Reading and writing to files](#reading-and-writing-to-files)
 * [Advanced functions on files](#advanced-functions-on-files)
 * [Accessing and traversing directories](#accessing-and-traversing-directories)
 * [File trees and diffs](#file-trees-and-diffs)


# Build Instructions

Please follow our [CMake project setup guide](https://github.com/cginternals/cmake-init/wiki/Setup-Guide)
to setup and build *cppfs*.


# Basic Examples

Opening files and checking for existence and type:

```C++
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

using namespace cppfs;

void openFile(const std::string & filename)
{
    FileHandle fh = fs::open(filename);

         if (fh.isFile())      { /* File ... */ }
    else if (fh.isDirectory()) { /* Directory ... */ }
    else if (!fh.exists())     { /* Not there ... */ }
}
```

Open a file for reading or writing:

```C++
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

using namespace cppfs;

void openFile(const std::string & filename)
{
    FileHandle fh = fs::open(filename);

    if (fh.isFile())
    {
        auto in = fh.createInputStream();
        // ...

        auto out = fh.createOutputStream();
        // ...
    }
}
```

Listing directory entries:

```C++
#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>

using namespace cppfs;

void lstDir(const std::string & path)
{
    FileHandle dir = fs::open(path);

    if (dir.isDirectory())
    {
        for (FileIterator it = dir.begin(); it != dir.end(); ++it)
        {
            std::string path = *it;
        }
    }
}
```


# Features

### Paths and URLs

The class *FilePath* is used to represent paths in the file system.
It can be constructed from a string and converted back into a string.

```C++
FilePath path("data/readme.txt");
std::string pathOut = path.path();
```

Paths are stored in a unified format using only forward slashes ('/')
as a separator. The unified format is compatible to all systems,
but as a convenience for the user, it should be converted to the
native format when displaying paths. To convert a path into the native
format, call *toNative*.

```C++
FilePath path("data/readme.txt");
std::cout << "File path: " << path.toNative() << std::endl;
```

A *FilePath* can be used to obtain syntactical information about a path.
It does however work purely on the string, it cannot provide information
about the actual file or directory the path points to. The following
functions are useful to get information about a path:

```C++
FilePath path = ...;

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
All of these functions ignore trailing slashes on the path, so they operate on
the object the path points to, not it contents.

```C++
FilePath path1("C:/path/to/file.txt");
FilePath path2("C:/path/to/directory/");

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
FilePath src("C:/projects");
FilePath rel("../documents/test.txt");
FilePath abs("C:/projects2/readme.txt");

FilePath path1 = src.resolve(rel);
std::cout << path1.fullPath() << std::endl; // "C:/projects/../documents/test.txt"

FilePath path2 = src.resolve(abs);
std::cout << path2.fullPath() << std::endl; // "C:/projects2/readme.txt"
```

When combining relative paths, the resulting strings may contain a lot
of ".." and "." components. To resolve these on a syntactical level, the
function *resolved* can be called. It will remove all occurences of "."
and "..", as long as it is possible. Occurences of ".." at the beginning
of a path will not be removed.

```C++
FilePath path("C:/projects/../documents/test.txt");
std::cout << path.resolved() << std::endl; // "C:/documents/test.txt"
```


### Accessing the file system

The main class for accessing files and directories in cppfs is *FileHandle*.
It can be used to get information about file system objects, to manipulate
them (e.g., copy, rename, or delete), as well as to read and write files.

To obtain a file handle, the global function *fs::open* can be called. The
type of file system that is accessed will be determined automatically by the
given path or URL. File systems will be closed automatically when there are
no longer any open file handles left.

```C++
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
*AbstractFileSystem* interface to access it.

```C++
class CustomFS : public AbstractFileSystem
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
FileHandle file = fs::open("data/readme.txt");

// Get a second handle to the same file
FileHandle file2 = file;
```

Once a file handle has been obtained, it can be used to query basic information
about the file system object is points to.

```C++
FileHandle file = fs::open("data/readme.txt");

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
FileHandle file = fs::open("data/readme.txt");

file.setUserId(1000);
file.setGroupId(1000);
file.setPermissions(FilePermissions::UserRead | FilePermissions::GroupRead);
```

The file information is retrieved only when needed, i.e., on the first call
of one of the above functions, and then cached in memory. When an operation on
the handle modifies the file information, it is updated automatically.
However, if a file is modified outside of the application or using a different
handle to the same file, the file handle cannot know about the change. Therefore,
it must be updated manually:

```C++
FileHandle file = fs::open("data/readme.txt");

// If the file has been modified outside the application ...
file.updateFileInfo();
```


### File operations

Using the *FileHandle*, basic file operations can be performed.
For binary operations, such as copy or move, the second file handle
is considered to be the target of the operation. Depending on the
operation, the target does not need to already exist. If the target
points to a directory rather than a file, the target will be considered
to be inside that directory (for example, a file will be copied into
the given directory).

```C++
FileHandle dir  = fs::open("data");
FileHandle file = fs::open("readme.txt");
FileHandle dest;

// Create directory if it does not yet exist
if (!dir.isDirectory()) dir.createDirectory();

// Copy file into directory
file.copy(dir);

// Copy file to another file
dest = dir.open("readme2.txt");
file.copy(dest);

// Rename file
file.rename("readme3.txt");

// Move file into directory
file.move(dir);

// Create hard link
dest = dir.open("readme4.txt");
file.createLink(dest);

// Create symbolic link
dest = dir.open("readme5.txt");
file.createSymbolicLink(dest);

// Delete file
file.remove();

// Delete directory (only if empty)
dir.removeDirectory();
```


### Reading and writing to files

To read and write files, standard C++ streams are applied. To open an input stream for a
file, call *createInputStream*. To create an output stream, call *createOutputStream*.

```C++
FileHandle file = fs::open("readme.txt");

std::unique_ptr<std::istream> in = file.createInputStream();
// ...

std::unique_ptr<std::ostream> out = file.createOutputStream();
// ...
```

For convience, there are also functions for reading and writing entire files using strings:

```C++
FileHandle file = fs::open("readme.txt");

std::string content = file.readFile();

file.writeFile("no more text ...");
```


### Advanced functions on files

Advanced file operation include the generation of hashes and base64 encoding of files.
They can be called directly on a file handle:

```C++
FileHandle file = fs::open("readme.txt");

// Get SHA1 hash of a file
std::string hash = file.sha1();

// Get base64 encoding of a file
std::string base64 = file.base64();

// Write file from base64 encoding
file.writeFileBase64(base64);
```


### Accessing and traversing directories

A *FileHandle* is used to access files as well as directories.
To check if a file handle points to a directory, the function
*isDirectory* can be used.

```C++
FileHandle dir = fs::open("data");
if (dir.isDirectory())
{
    // ...
}
```

To list all files in a directory, call *listFiles*:

```C++
FileHandle dir = fs::open("data");
std::vector<std::string> files = dir.listFiles();
```

For better control, the C++ iterator interface can be used:

```C++
FileHandle dir = fs::open("data");
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
FileHandle dir = fs::open("data");

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

When a handle to a directory has been obtained, it can
be used to open file handles relative to that directory:

```C++
FileHandle dir = fs::open("data");

// Open parent directory
FileHandle parentDir = dir.parentDirectory();

// Open file in directory
FileHandle file1 = dir.open("readme.txt");

// Open file relative to directory
FileHandle file2 = dir.open("../readme.txt");
```


### File trees and diffs

For higher level operations on directory trees, the classes *Tree*, *Diff*,
and *Change* can be used. A *Tree* contains the information about all files
and directories in a tree data structure. It can be obtained from a
directory handle by calling *readTree*.

```C++
// Read directory tree from current directory
std::unique_ptr<Tree> tree1 = dir.readTree();

// Read directory tree from current directory, giving it a virtual root
std::unique_ptr<Tree> tree2 = dir.readTree("/root");
```

Given two directory trees, the differences between them can be calculated,
resulting in a diff object. A diff contains a number of file system operations
that need to be performed in order to transform from the source tree to
the destination tree. This can be utilized to implement basic file syncing:

```C++
// Open directories
FileHandle srcDir = fs::open("data1");
FileHandle dstDir = fs::open("data2");

// Read both directory trees
auto srcTree = srcDir.readTree();
auto dstTree = dstDir.readTree();

// Calculate differences
auto diff = dstTree->createDiff(*srcTree);

// Sync directories
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
```
