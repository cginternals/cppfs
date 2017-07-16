
# 
# External dependencies
# 

find_package(LibSSH2 REQUIRED)
find_package(LibCrypto)
find_package(ZLIB)
find_package(OpenSSL)


# 
# Library name and options
# 

# Target name
set(target cppfs)
message(STATUS "Lib ${target}")

# Set API export file and macro
string(MAKE_C_IDENTIFIER ${target} target_id)
string(TOUPPER ${target_id} target_id)
set(feature_file "include/${target}/${target}_features.h")
set(export_file  "include/${target}/${target}_api.h")
set(export_macro "${target_id}_API")


# 
# Sources
# 

set(include_path "${CMAKE_CURRENT_SOURCE_DIR}/include/${target}")
set(source_path  "${CMAKE_CURRENT_SOURCE_DIR}/source")

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    set(localfs "windows")
else()
    set(localfs "posix")
endif()

set(headers
    ${include_path}/cppfs.h
    ${include_path}/system.h
    ${include_path}/fs.h
    ${include_path}/FileHandle.h
    ${include_path}/FileIterator.h
    ${include_path}/FileVisitor.h
    ${include_path}/FunctionalFileVisitor.h
    ${include_path}/AbstractFileSystem.h
    ${include_path}/AbstractFileHandleBackend.h
    ${include_path}/AbstractFileIteratorBackend.h
    ${include_path}/InputStream.h
    ${include_path}/OutputStream.h
    ${include_path}/LoginCredentials.h
    ${include_path}/FilePath.h
    ${include_path}/Url.h
    ${include_path}/Tree.h
    ${include_path}/Diff.h
    ${include_path}/Change.h
    ${include_path}/units.h

    ${include_path}/${localfs}/LocalFileSystem.h
    ${include_path}/${localfs}/LocalFileHandle.h
    ${include_path}/${localfs}/LocalFileIterator.h

    ${include_path}/ssh/SshFileSystem.h
    ${include_path}/ssh/SshFileHandle.h
    ${include_path}/ssh/SshFileIterator.h
    ${include_path}/ssh/SshInputStreamBuffer.h
    ${include_path}/ssh/SshOutputStreamBuffer.h
)

set(sources
    ${source_path}/system.cpp
    ${source_path}/fs.cpp
    ${source_path}/FileHandle.cpp
    ${source_path}/FileIterator.cpp
    ${source_path}/FileVisitor.cpp
    ${source_path}/FunctionalFileVisitor.cpp
    ${source_path}/AbstractFileSystem.cpp
    ${source_path}/AbstractFileHandleBackend.cpp
    ${source_path}/AbstractFileIteratorBackend.cpp
    ${source_path}/InputStream.cpp
    ${source_path}/OutputStream.cpp
    ${source_path}/LoginCredentials.cpp
    ${source_path}/FilePath.cpp
    ${source_path}/Url.cpp
    ${source_path}/Tree.cpp
    ${source_path}/Diff.cpp
    ${source_path}/Change.cpp

    ${source_path}/${localfs}/LocalFileSystem.cpp
    ${source_path}/${localfs}/LocalFileHandle.cpp
    ${source_path}/${localfs}/LocalFileIterator.cpp

    ${source_path}/ssh/SshFileSystem.cpp
    ${source_path}/ssh/SshFileHandle.cpp
    ${source_path}/ssh/SshFileIterator.cpp
    ${source_path}/ssh/SshInputStreamBuffer.cpp
    ${source_path}/ssh/SshOutputStreamBuffer.cpp
)

# Group source files
set(header_group "Header Files (API)")
set(source_group "Source Files")
source_group_by_path(${include_path} "\\\\.h$|\\\\.hpp$" 
    ${header_group} ${headers})
source_group_by_path(${source_path}  "\\\\.cpp$|\\\\.c$|\\\\.h$|\\\\.hpp$" 
    ${source_group} ${sources})


# 
# Create library
# 

# Build library
add_library(${target}
    ${sources}
    ${headers}
)

# Create namespaced alias
add_library(${META_PROJECT_NAME}::${target} ALIAS ${target})

# Export library for downstream projects
export(TARGETS ${target} NAMESPACE ${META_PROJECT_NAME}:: FILE ${PROJECT_BINARY_DIR}/cmake/${target}/${target}-export.cmake)

# Create API export header
generate_export_header(${target}
    EXPORT_FILE_NAME  ${export_file}
    EXPORT_MACRO_NAME ${export_macro}
)


# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
    VERSION ${META_VERSION}
    SOVERSION ${META_VERSION_MAJOR}
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    ${PROJECT_BINARY_DIR}/source/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
    ${LIBSSH2_INCLUDE_DIR}

    PUBLIC
    ${DEFAULT_INCLUDE_DIRECTORIES}

    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${OPENSSL_LIBRARIES}
    ${LIBSSH2_LIBRARY}

    PUBLIC
    ${DEFAULT_LIBRARIES}

    INTERFACE
)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    target_link_libraries(${target}
        PRIVATE
        wsock32.lib
        ws2_32.lib
    )
endif()

if(LibCrypto_FOUND)
    target_link_libraries(${target}
        PRIVATE
        ${LIBCRYPTO_LIBRARY}
    )
endif()

if(ZLIB_FOUND)
    target_link_libraries(${target}
        PRIVATE
        ${ZLIB_LIBRARY}
    )
endif()

# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE

    PUBLIC
    $<$<NOT:$<BOOL:${BUILD_SHARED_LIBS}>>:${target_id}_STATIC_DEFINE>
    ${DEFAULT_COMPILE_DEFINITIONS}

    INTERFACE
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}

    INTERFACE
)


# 
# Target Health
# 

perform_health_checks(
    ${target}
    ${sources}
    ${headers}
)


# 
# Deployment
# 

# Library
install(TARGETS ${target}
    EXPORT  "${target}-export"            COMPONENT dev
    RUNTIME DESTINATION ${INSTALL_BIN}    COMPONENT runtime
    LIBRARY DESTINATION ${INSTALL_SHARED} COMPONENT runtime
    ARCHIVE DESTINATION ${INSTALL_LIB}    COMPONENT dev
)

# Header files
install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# Generated header files
install(DIRECTORY
    ${CMAKE_CURRENT_BINARY_DIR}/include/${target} DESTINATION ${INSTALL_INCLUDE}
    COMPONENT dev
)

# CMake config
install(EXPORT ${target}-export
    NAMESPACE   ${META_PROJECT_NAME}::
    DESTINATION ${INSTALL_CMAKE}/${target}
    COMPONENT   dev
)
