
#include <cppfs/FileHandle.h>

#include <iostream>
#include <sstream>
#include <iterator>
#include <array>

#if defined(__APPLE__)
    #define COMMON_DIGEST_FOR_OPENSSL
    #include <CommonCrypto/CommonDigest.h>
    #define SHA1 CC_SHA1
#elif defined(CPPFS_USE_OpenSSL)
    #include <openssl/sha.h>
#endif

#include <basen/basen.hpp>

#include <cppfs/fs.h>
#include <cppfs/FilePath.h>
#include <cppfs/FileIterator.h>
#include <cppfs/FileVisitor.h>
#include <cppfs/FunctionalFileVisitor.h>
#include <cppfs/FileWatcher.h>
#include <cppfs/Tree.h>
#include <cppfs/AbstractFileSystem.h>
#include <cppfs/AbstractFileHandleBackend.h>
#include <cppfs/AbstractFileIteratorBackend.h>


namespace cppfs
{


FileHandle::FileHandle()
: m_backend(nullptr)
{
}

FileHandle::FileHandle(std::unique_ptr<AbstractFileHandleBackend> && backend)
: m_backend(std::move(backend))
{
}

FileHandle::FileHandle(const FileHandle & fileHandle)
: m_backend(fileHandle.m_backend ? fileHandle.m_backend->clone() : nullptr)
{
}

FileHandle::FileHandle(FileHandle && fileHandle)
: m_backend(std::move(fileHandle.m_backend))
{
}

FileHandle::~FileHandle()
{
}

FileHandle & FileHandle::operator=(const FileHandle & fileHandle)
{
    if (fileHandle.m_backend)
    {
        m_backend = fileHandle.m_backend->clone();
    }
    else
    {
        m_backend.reset(nullptr);
    }

    return *this;
}

FileHandle & FileHandle::operator=(FileHandle && fileHandle)
{
    m_backend = std::move(fileHandle.m_backend);

    return *this;
}

AbstractFileSystem * FileHandle::fs() const
{
    return m_backend ? m_backend->fs() : nullptr;
}

std::string FileHandle::path() const
{
    return m_backend ? m_backend->path() : "";
}

std::string FileHandle::fileName() const
{
    return m_backend ? FilePath(m_backend->path()).fileName() : "";
}

void FileHandle::updateFileInfo()
{
    if (m_backend) m_backend->updateFileInfo();
}

bool FileHandle::exists() const
{
    return m_backend ? m_backend->exists() : false;
}

bool FileHandle::isFile() const
{
    return m_backend ? m_backend->isFile() : false;
}

bool FileHandle::isDirectory() const
{
    return m_backend ? m_backend->isDirectory() : false;
}

bool FileHandle::isSymbolicLink() const
{
    return m_backend ? m_backend->isSymbolicLink() : false;
}

std::vector<std::string> FileHandle::listFiles() const
{
    return m_backend ? m_backend->listFiles() : std::vector<std::string>();
}

void FileHandle::traverse(VisitFunc funcFileEntry)
{
    FunctionalFileVisitor visitor(funcFileEntry);
    traverse(visitor);
}

void FileHandle::traverse(VisitFunc funcFile, VisitFunc funcDirectory)
{
    FunctionalFileVisitor visitor(funcFile, funcDirectory);
    traverse(visitor);
}

void FileHandle::traverse(FileVisitor & visitor)
{
    // Check if file or directory exists
    if (!exists())
    {
        return;
    }

    // Invoke visitor
    bool traverseSubDir = visitor.onFileEntry(*this);

    // Is this is directory?
    if (isDirectory() && traverseSubDir)
    {
        // Iterator over child entries
        for (auto it = begin(); it != end(); ++it)
        {
            // Open file or directory
            FileHandle fh = open(*it);
            if (!fh.exists()) continue;

            // Handle entry
            fh.traverse(visitor);
        }
    }
}

std::unique_ptr<Tree> FileHandle::readTree(const std::string & path, bool includeHash) const
{
    // Check if file or directory exists
    if (!exists())
    {
        return nullptr;
    }

    // Create tree
    auto tree = std::unique_ptr<Tree>(new Tree);
    tree->setPath(path);
    tree->setFileName(fileName());
    tree->setDirectory(isDirectory());
    tree->setSize(size());
    tree->setAccessTime(accessTime());
    tree->setModificationTime(modificationTime());
    tree->setUserId(userId());
    tree->setGroupId(groupId());
    tree->setPermissions(permissions());

    if (includeHash)
    {
        tree->setSha1(sha1());
    }

    // Is this is directory?
    if (isDirectory())
    {
        // Add children
        for (auto it = begin(); it != end(); ++it)
        {
            // Open file or directory
            FileHandle fh = open(*it);
            if (!fh.exists()) continue;

            // Compose name
            std::string subName = path;
            if (!subName.empty()) subName += "/";
            subName += fh.fileName();

            // Read subtree
            auto subTree = fh.readTree(subName, includeHash);

            // Add subtree to list
            if (subTree)
            {
                tree->add(std::move(subTree));
            }
        }
    }

    // Return tree
    return tree;
}

FileIterator FileHandle::begin() const
{
    return m_backend ? FileIterator(m_backend->begin()) : FileIterator();
}

FileIterator FileHandle::end() const
{
    return FileIterator();
}

unsigned int FileHandle::size() const
{
    return m_backend ? m_backend->size() : 0;
}

unsigned int FileHandle::accessTime() const
{
    return m_backend ? m_backend->accessTime() : 0;
}

unsigned int FileHandle::modificationTime() const
{
    return m_backend ? m_backend->modificationTime() : 0;
}

unsigned int FileHandle::userId() const
{
    return m_backend ? m_backend->userId() : 0;
}

void FileHandle::setUserId(unsigned int uid)
{
    if (m_backend) m_backend->setUserId(uid);
}

unsigned int FileHandle::groupId() const
{
    return m_backend ? m_backend->groupId() : 0;
}

void FileHandle::setGroupId(unsigned int gid)
{
    if (m_backend) m_backend->setGroupId(gid);
}

unsigned long FileHandle::permissions() const
{
    return m_backend ? m_backend->permissions() : 0;
}

void FileHandle::setPermissions(unsigned long permissions)
{
    if (m_backend) m_backend->setPermissions(permissions);
}

std::string FileHandle::sha1() const
{
    // Check file
    if (!isFile())
    {
        return "";
    }

#ifdef CPPFS_USE_OpenSSL
    // Open file
    auto inputStream = createInputStream();
    if (!inputStream)
    {
        return "";
    }

    // Initialize hash
    unsigned char hash[20];
    SHA_CTX context;
    SHA1_Init(&context);

    // Read whole while
    while (!inputStream->eof())
    {
        // Read a maximum of 1024 bytes at once
        // Read data from file
        std::array<char, 1024> buf;
        inputStream->read(buf.data(), buf.size());

        size_t count = inputStream->gcount();
        if (count > 0)
        {
            // Update hash
            SHA1_Update(&context, buf.data(), count);
        } else break;
    }

    // Compute hash
    SHA1_Final(hash, &context);
    return fs::hashToString(hash);
#else
    return "";
#endif
}

std::string FileHandle::base64() const
{
    // Check file
    if (!isFile())
    {
        return "";
    }

    // Open file
    auto inputStream = createInputStream();

    if (!inputStream)
    {
        return "";
    }

    // Create stream iterator
    std::noskipws(*inputStream);
    std::istream_iterator<char> it(*inputStream);
    std::istream_iterator<char> end;

    // Encode base64
    std::string base64;
    bn::encode_b64(it, end, back_inserter(base64));

    // Return encoded string
    return base64;
}

FileHandle FileHandle::parentDirectory() const
{
    return m_backend ? m_backend->fs()->open(FilePath(m_backend->path()).resolve("..").resolved()) : FileHandle();
}

FileHandle FileHandle::open(const std::string & path) const
{
    return m_backend ? m_backend->fs()->open(FilePath(m_backend->path()).resolve(path).fullPath()) : FileHandle();
}

bool FileHandle::createDirectory()
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // Make directory
    if (!m_backend->createDirectory())
    {
        return false;
    }

    // Done
    return true;
}

bool FileHandle::removeDirectory()
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // Remove directory
    if (!m_backend->removeDirectory())
    {
        return false;
    }

    // Done
    return true;
}

void FileHandle::copyDirectoryRec(FileHandle & dstDir)
{
    // Check if source directory is valid
    if (!isDirectory())
    {
        return;
    }

    // Check destination directory and try to create it if necessary
    if (!dstDir.isDirectory())
    {
        dstDir.createDirectory();

        if (!dstDir.isDirectory())
        {
            return;
        }
    }

    // Copy all entries
    for (auto it = begin(); it != end(); ++it)
    {
        std::string filename = *it;

        FileHandle src = this->open(filename);
        FileHandle dst = dstDir.open(filename);

        if (src.isDirectory())
        {
            src.copyDirectoryRec(dst);
        }

        else if (src.isFile())
        {
            src.copy(dst);
        }
    }
}

void FileHandle::removeDirectoryRec()
{
    // Check directory
    if (!isDirectory()) {
        return;
    }

    // Delete all entries
    for (auto it = begin(); it != end(); ++it)
    {
        FileHandle fh = open(*it);

        if (fh.isDirectory())
        {
            fh.removeDirectoryRec();
        }

        else if (fh.isFile())
        {
            fh.remove();
        }
    }

    // Remove directory
    removeDirectory();
}

bool FileHandle::copy(FileHandle & dest)
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // If both handles are from the same file system, use internal method
    if (m_backend->fs() == dest.m_backend->fs())
    {
        bool result = m_backend->copy(*dest.m_backend.get());
        dest.updateFileInfo();
        return result;
    }

    // Otherwise, use generic (slow) method
    else
    {
        return genericCopy(dest);
    }
}

bool FileHandle::move(FileHandle & dest)
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // If both handles are from the same file system, use internal method
    if (m_backend->fs() == dest.m_backend->fs())
    {
        bool result = m_backend->move(*dest.m_backend.get());
        dest.updateFileInfo();
        return result;
    }

    // Otherwise, use generic (slow) method
    else
    {
        return genericMove(dest);
    }
}

bool FileHandle::createLink(FileHandle & dest)
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // If both handles are from the same file system, use internal method
    if (m_backend->fs() == dest.m_backend->fs())
    {
        bool result = m_backend->createLink(*dest.m_backend.get());
        dest.updateFileInfo();
        return result;
    }

    // Otherwise, this is not possible
    else
    {
        return false;
    }
}

bool FileHandle::createSymbolicLink(FileHandle & dest)
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // If both handles are from the same file system, use internal method
    if (m_backend->fs() == dest.m_backend->fs())
    {
        bool result = m_backend->createSymbolicLink(*dest.m_backend.get());
        dest.updateFileInfo();
        return result;
    }

    // Otherwise, this is not possible
    else
    {
        return false;
    }
}

bool FileHandle::rename(const std::string & filename)
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // Rename file
    if (!m_backend->rename(filename))
    {
        return false;
    }

    // Done
    return true;
}

bool FileHandle::remove()
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // Remove file
    if (!m_backend->remove())
    {
        return false;
    }

    // Update file information
    return true;
}

FileWatcher FileHandle::watch(unsigned int events, RecursiveMode recursive)
{
    // Create file system watcher
    FileWatcher watcher(fs());
    watcher.add(*this, events, recursive);

    // Return watcher
    return std::move(watcher);
}

std::unique_ptr<std::istream> FileHandle::createInputStream(std::ios_base::openmode mode) const
{
    // Check backend
    if (!m_backend)
    {
        return nullptr;
    }

    // Return stream
    return m_backend->createInputStream(mode);
}

std::unique_ptr<std::ostream> FileHandle::createOutputStream(std::ios_base::openmode mode)
{
    // Check backend
    if (!m_backend)
    {
        return nullptr;
    }

    // Return stream
    return m_backend->createOutputStream(mode);
}

std::string FileHandle::readFile() const
{
    // Check if file exists
    if (isFile())
    {
        // Open input stream
        auto inputStream = createInputStream();
        if (!inputStream) return "";

        // Read content
        std::stringstream buffer;
        buffer << inputStream->rdbuf();

        // Return string
        return buffer.str();
    }

    // Error, not a valid file
    return "";
}

bool FileHandle::writeFile(const std::string & content)
{
    // Open output stream
    auto outputStream = createOutputStream();
    if (!outputStream) return false;

    // Write content to file
    (*outputStream) << content;

    // Done
    return true;
}

bool FileHandle::writeFileBase64(const std::string & base64)
{
    // Open output stream
    auto outputStream = createOutputStream();
    if (!outputStream) return false;

    // Write content to file
    (*outputStream) << fs::fromBase64(base64);

    // Done
    return true;
}

bool FileHandle::genericCopy(FileHandle & dest)
{
    // Check backend
    if (!m_backend)
    {
        return false;
    }

    // Open files
    auto in  = createInputStream(std::ios::binary);
    auto out = dest.createOutputStream(std::ios::binary | std::ios::trunc);

    if (!in || !out)
    {
        // Error!
        return false;
    }

    // Copy file
    (*out) << in->rdbuf();
    out->flush();

    // Reload information on destination file
    dest.updateFileInfo();

    // Done
    return true;
}

bool FileHandle::genericMove(FileHandle & dest)
{
    // Copy file
    if (!genericCopy(dest))
    {
        return false;
    }

    // Remove source file
    return remove();
}


} // name cppfs
