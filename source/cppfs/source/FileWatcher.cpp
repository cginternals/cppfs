
#include <cppfs/FileWatcher.h>

#include <iostream>

#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/AbstractFileSystem.h>
#include <cppfs/null/NullFileWatcher.h>


namespace cppfs
{


FileWatcher::FileWatcher()
: m_backend(fs::localFS()->createFileWatcher(*this))
{
}

FileWatcher::FileWatcher(AbstractFileSystem * fs)
: m_backend(fs ? fs->createFileWatcher(*this) : nullptr)
{
}

FileWatcher::FileWatcher(FileWatcher && fileWatcher)
: m_backend(std::move(fileWatcher.m_backend))
{
}

FileWatcher::~FileWatcher()
{
}

FileWatcher & FileWatcher::operator=(FileWatcher && fileWatcher)
{
    m_backend = std::move(fileWatcher.m_backend);

    return *this;
}

AbstractFileSystem * FileWatcher::fs() const
{
    return m_backend ? m_backend->fs() : nullptr;
}

void FileWatcher::add(const FileHandle & fileHandle, unsigned int mode)
{
    // Check backend
    if (!m_backend) {
        return;
    }

    // Check that file handle belongs to the same file system as the watcher
    if (fileHandle.fs() != fs()) {
        return;
    }

    // Add file to watcher
    m_backend->add(fileHandle.path(), mode);
}

void FileWatcher::watch()
{
    // Check backend
    if (!m_backend) {
        return;
    }

    // Watch files
    m_backend->watch();
}

void FileWatcher::onFileEvent(const std::string & path, FileEvent event)
{
    // [TODO] Pass FileHandle instead of string
    // [TODO] Make sure that path is fully qualified
    // [TODO] Add methods to register callback functions

    // Open file
    FileHandle fh = fs::open(path);

    // Get file type
    std::string type = (fh.isDirectory() ? "Directory" : "File");

    // Get operation
    std::string operation = ( (event & FileCreated) ? "created" :
                            ( (event & FileRemoved) ? "removed" :
                                                      "modified" ) );

    // Log event
    std::cout << type << " '" << path << "' has been " << operation << "." << std::endl;
}


} // name cppfs
