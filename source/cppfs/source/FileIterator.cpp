
#include <cppfs/FileIterator.h>

#include <cppfs/AbstractFileIteratorBackend.h>


namespace cppfs
{


FileIterator::FileIterator()
{
}

FileIterator::FileIterator(std::unique_ptr<AbstractFileIteratorBackend> && backend)
: m_backend(std::move(backend))
{
}

FileIterator::FileIterator(const FileIterator & fileIterator)
: m_backend(fileIterator.m_backend ? fileIterator.m_backend->clone() : nullptr)
{
}

FileIterator::FileIterator(FileIterator && fileIterator)
: m_backend(std::move(fileIterator.m_backend))
{
}

FileIterator::~FileIterator()
{
}

FileIterator & FileIterator::operator=(const FileIterator & fileIterator)
{
    if (fileIterator.m_backend)
    {
        m_backend = fileIterator.m_backend->clone();
    }
    else
    {
        m_backend.reset(nullptr);
    }

    return *this;
}

std::string FileIterator::operator*() const
{
    return m_backend ? m_backend->name() : "";
}

void FileIterator::operator++()
{
    if (m_backend)
    {
        m_backend->next();
    }
}

bool FileIterator::operator==(const FileIterator & it) const
{
    bool valid1 = false;
    bool valid2 = false;
    AbstractFileSystem * fs1 = nullptr;
    AbstractFileSystem * fs2 = nullptr;
    std::string path1;
    std::string path2;
    int  index1 = -1;
    int  index2 = -1;

    // Check if both iterators operate on the same file system
    fs1 = (   m_backend ?    m_backend->fs() : nullptr);
    fs2 = (it.m_backend ? it.m_backend->fs() : nullptr);

    // Get status of first iterator
    if (m_backend && m_backend->valid())
    {
        valid1 = true;
        index1 = m_backend->index();
        path1  = m_backend->path();
    }

    // Get status of second iterator
    if (it.m_backend && it.m_backend->valid())
    {
        valid2 = true;
        index2 = it.m_backend->index();
        path2  = it.m_backend->path();
    }

    // If both iterators are valid, check if they are equal
    if (valid1 && valid2)
    {
        return fs1 == fs2 && path1 == path2 && index1 == index2;
    }

    // Otherwise, consider them equal if both are invalid
    else
    {
        return valid1 == valid2;
    }
}

bool FileIterator::operator!=(const FileIterator & it) const
{
    return !((*this) == it);
}

AbstractFileSystem * FileIterator::fs() const
{
    return m_backend ? m_backend->fs() : nullptr;
}


} // namespace cppfs
