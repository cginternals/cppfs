
#include <cppfs/Tree.h>

#include <algorithm>
#include <iostream>

#include <cppfs/fs.h>
#include <cppfs/FileHandle.h>
#include <cppfs/Diff.h>


namespace cppfs
{


Tree::Tree()
: m_directory(false)
, m_size(0)
, m_accessTime(0)
, m_modificationTime(0)
, m_userId(0)
, m_groupId(0)
, m_permissions(0)
{
}

Tree::~Tree()
{
    clear();
}

void Tree::clear()
{
    m_children.clear();
}

const std::string & Tree::path() const
{
    return m_path;
}

void Tree::setPath(const std::string & path)
{
    m_path = path;
}

void Tree::setPath(std::string && path)
{
    m_path = std::move(path);
}

const std::string & Tree::fileName() const
{
    return m_filename;
}

void Tree::setFileName(const std::string & filename)
{
    m_filename = filename;
}

void Tree::setFileName(std::string && filename)
{
    m_filename = filename;
}

bool Tree::isFile() const
{
    return !m_directory;
}

bool Tree::isDirectory() const
{
    return m_directory;
}

void Tree::setDirectory(bool isDir)
{
    m_directory = isDir;
}

unsigned int Tree::size() const
{
    return m_size;
}

void Tree::setSize(unsigned int size)
{
    m_size = size;
}

unsigned int Tree::accessTime() const
{
    return m_accessTime;
}

void Tree::setAccessTime(unsigned int time)
{
    m_accessTime = time;
}

unsigned int Tree::modificationTime() const
{
    return m_modificationTime;
}

void Tree::setModificationTime(unsigned int time)
{
    m_modificationTime = time;
}

unsigned int Tree::userId() const
{
    return m_userId;
}

void Tree::setUserId(unsigned int id)
{
    m_userId = id;
}

unsigned int Tree::groupId() const
{
    return m_groupId;
}

void Tree::setGroupId(unsigned int id)
{
    m_groupId = id;
}

unsigned long Tree::permissions() const
{
    return m_permissions;
}

void Tree::setPermissions(unsigned int permissions)
{
    m_permissions = permissions;
}

const std::string & Tree::sha1() const
{
    return m_sha1;
}

void Tree::setSha1(const std::string & hash)
{
    m_sha1 = hash;
}

void Tree::setSha1(std::string && hash)
{
    m_sha1 = std::move(hash);
}

std::vector<std::string> Tree::listFiles() const
{
    std::vector<std::string> children;

    if (!m_directory)
    {
        return children;
    }

    for (auto & tree : m_children)
    {
        children.push_back(tree->fileName());
    }

    return children;
}

const std::vector< std::unique_ptr<Tree> > & Tree::children() const
{
    return m_children;
}

std::vector< std::unique_ptr<Tree> > & Tree::children()
{
    return m_children;
}

void Tree::add(std::unique_ptr<Tree> && tree)
{
    // Check parameters
    if (!tree)
    {
        return;
    }

    // Add tree to list
    m_children.push_back(std::move(tree));
}

void Tree::print(const std::string & indent) const
{
    std::cout << indent << m_filename << std::endl;

    for (auto & tree : m_children)
    {
        tree->print(indent + "  ");
    }
}

std::unique_ptr<Diff> Tree::createDiff(const Tree & target) const
{
    auto diff = new Diff;

    createDiff(this, &target, *diff);

    return std::unique_ptr<Diff>(diff);
}

Tree::Tree(const Tree &)
{
}

const Tree & Tree::operator=(const Tree &)
{
    return *this;
}

void Tree::createDiff(const Tree * currentState, const Tree * targetState, Diff & diff)
{
    // Check target state
    if (!targetState || !targetState->isDirectory())
    {
        return;
    }

    // Check if destination directory exists in current state
    if (!currentState)
    {
        // Copy directory recursively
        diff.add(Change::CopyDir, targetState->path());
        return;
    }

    // List files in current and target state
    auto targetFiles  = targetState->listFiles();
    auto currentFiles = currentState->listFiles();

    // Delete files which are in the current state but not in the target state
    for (const auto & file : currentState->children())
    {
        // Check if file is in the target state
        if (std::find(targetFiles.begin(), targetFiles.end(), file->fileName()) == targetFiles.end())
        {
            // Check type
            if (file->isDirectory())
            {
                // Delete directory recursively
                diff.add(Change::RemoveDir, file->path());
            }

            else
            {
                // Delete file
                diff.add(Change::RemoveFile, file->path());
            }
        }
    }

    // Copy files which are new or changed
    for (const auto & targetFile : targetState->children())
    {
        // Get file in the current state
        const auto it = std::find(currentFiles.begin(), currentFiles.end(), targetFile->fileName());
        Tree * currentFile = (it != currentFiles.end()) ? currentState->children()[std::distance(currentFiles.begin(), it)].get() : nullptr;

        // Directory
        if (targetFile->isDirectory())
        {
            // Sync directories recursively
            createDiff(currentFile, targetFile.get(), diff);
        }

        // File
        else if (targetFile->isFile())
        {
            // Check if file needs to be updated
            bool needsUpdate = (currentFile == nullptr);

            if (!needsUpdate)
            {
                needsUpdate = (targetFile->size() != currentFile->size());
            }

            if (!needsUpdate)
            {
                needsUpdate = (targetFile->sha1() != currentFile->sha1());
            }

            if (needsUpdate)
            {
                // Copy file
                diff.add(Change::CopyFile, targetFile->path());
            }
        }
    }
}


} // namespace cppfs
