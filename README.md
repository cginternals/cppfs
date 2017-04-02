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

using namespace cppfs;

void openFile(const std::string & filename)
{
    FileHandle f = fs::open(filename);

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


## Get absolute path to the current executable

```cpplocate``` offers a cross-platform implementation to obtain the location of the current executable
in most cases. This can be used by the application itself to locate its data.

## Module information files

Module information files contain meta-information about a module, such as project name,
version, and vendor. They also provide the information of where to find data assets for
that module based on the location of the module information file itself, or, in some cases,
using an absolute path.

A module information file has the filename ```<projectname>.modinfo``` and contains an arbitrary
number of key/value-pairs, e.g.:

```cmake
  name: examplelib
  version: 1.0.0
  description: Example library
  author: Example organization
  dataPath: ${ModulePath}/data
```

The keys are purely conventional and can be used in any way as defined by the using application.
To express file paths relative to the module information file, the placeholder variable ```${ModulePath}```
can be used. When loading the module information, this variable will be replaced with the path
to the directory containing the module information file.

## CMake integration

A CMake module is provided for creating module information files automatically, taking into account
different situations such as build-time (finding modules and data in a development tree) and
install-time (finding modules and data from an installed location).

Use ```find_package(cpplocate)``` to find the cpplocate library, pointing ```CMAKE_PREFIX_PATH``` to the
directory that contains ```cpplocate```. This will locate the library and also include the necessary
cmake functions into your project.

The following cmake functions are provided by ```cpplocate```:

### ```generate_module_info```: Define a module information file.

```cmake
generate_module_info(<project_id>
    VALUES
    [<key> <value>]*

    BUILD_VALUES
    [<key> <value>]*

    INSTALL_VALUES
    [<key> <value>]*
)
```

Define a module information file for a module named by ```project_id```. The module name can be chosen
arbitrarily and can, but does not need to, match the name of the project or a library. The
filename will be derived as ```<project_id>.modinfo```.

The generated module information file will contain all key/value-pairs specified after ```VALUES```.
When created into the build-directory of the current development tree, the values specified
after ```BUILD_VALUES``` will also be added. But when installed using the cmake ```INSTALL```-target, the
values after ```INSTALL_VALUES``` will be used instead. This allows for providing different values
in the development tree and in installed location, e.g., the data path might point to the
absolute path in the development tree, but be defined relative to the module file on install.

Example usage:

```cmake
generate_module_info(examplelib
    VALUES
    name        "examplelib"
    version     "1.0.0"
    description "Example library"
    author      "Example organization"

    BUILD_VALUES
    dataPath    "${PROJECT_SOURCE_DIR}/data"

    INSTALL_VALUES
    dataPath    "\${ModulePath}/data"
)
```

### ```export_module_info```: Create module information file in build directory.

```cmake
export_module_info(<project_id>
    TARGET <target>
    [ FOLDER <folder>
      [RENAME <filename>]
    ]
)
```

Creates the actual module information file for a module named by ```project_id``` in the output directory of target ```<target>```. It uses the values from ```VALUES``` and ```BUILD_VALUES```. This is executed at build-time, providing a target named ```<target>-modinfo```. If ```FOLDER``` is specified, the target is put into the UI folder named by ```folder```. If ```RENAME``` is specified, the target output name is set as ```<filename>```, ```filename``` can contain generator expressions.

Example usage:

```cmake
export_module_info(examplelib TARGET examplelib FOLDER "cmake")
```

### ```export_module_info_with_deps```: Create module information file and copy module information files of dependencies into the build directory

```cmake
export_module_info_with_deps(<target>
    [ FOLDER <folder>
      [RENAME <filename>]
    ]
    REQUIRES <module_name> <import_target> [ <module_name> <import_target> ]*
)
```

Creates the actual module information file for a module named by ```<target>``` in the output directory of that target. It uses the values from ```VALUES``` and ```BUILD_VALUES```. This is executed at build-time, providing a target named ```<target>-modinfo```. If ```FOLDER``` is specified, the target is put into the UI folder named by ```folder```. If ```RENAME``` is specified, the target output name is set as ```<filename>```, ```filename``` can contain generator expressions. REQUIRES specifies a list of dependencies, whose module information files will be located and copied into the output directory.

To locate the module information file, a file named ```<module_name>.modinfo``` is located in the directory of the given target ```<import_target>```. Therefore, the module file must reside in the same directory as the library for this mechanism to work.

Example usage:

```cmake
export_module_info_with_deps(examplelib TARGET examplelib FOLDER "cmake")
```

### ```install_module_info```: Install modinfo file to target path.

```cmake
install_module_info(<project_id>
    DESTINATION <dest>
    [COMPONENT <component>]
    [RENAME <filename>]
)
```

Creates an installation rule to install a module information file named by ```project_id```. It uses the values from ```VALUES``` and ```INSTALL_VALUES```. The destination location is specified by ```dest```. If ```RENAME``` is specified, the file will be renamed to ```<filename>``` on installation, ```filename``` can contain generator expressions. If ```COMPONENT``` is specified, the module information file is added to the specified installation component.

Example usage:

```cmake
install_module_info(examplelib DESTINATION "." COMPONENT dev)
```

### ```copy_module_info```: Copy module information file to a specific location at build-time.

```cmake
copy_module_info(<project_id> <filename>)
```

This function writes the module information file named by ```project_id``` to an output file specified by ```filename``` at build-time, ```filename``` can contain generator expressions. It uses the values from ```VALUES``` and ```BUILD_VALUES```.

## Resolve dependend modules

Similar to a dynamic linker, ```cpplocate``` can resolve dependencies to other modules by locating
module information files. The search for modules is conducted in the following order:

1. in the directory of the current executable (not the working directory!)

2. in the directories provided by the environment variable ```CPPLOCATE_PATH```
   - ```<path>/<module>-info.modinfo```
   - ```<path>/<module>/<module>-info.modinfo```

3. in standard locations:
   - ```C:\Program Files\<module>\<module>-info.modinfo```
   - ```/usr/share/<module>/<module>-info.modinfo```
   - ```/usr/local/share/<module>/<module>-info.modinfo```

This functionality can be used by a library to locate its own data at runtime.
If it is used as a dependency for another project, such as an application or a
plugin using the library, it cannot rely on the data being relative to the
current executable or even the working directory. Therefore, the library can
use ```cpplocate``` to locate itself:

```C++
namespace examplelib
{

std::string determineDataPath()
{
    const cpplocate::ModuleInfo moduleInfo = cpplocate::findModule("examplelib");
    const std::string moduleInfoPath = moduleInfo.value("dataPath");

    return moduleInfoPath.empty() ? "data" : moduleInfoPath;
}

const std::string & dataPath()
{
    static const auto path = determineDataPath();

    return path;
}

}
```

And whenever data needs to be accessed, code like the following should be used:

```C++
std::string filename = dataPath() + "/textures/logo.png";
```

