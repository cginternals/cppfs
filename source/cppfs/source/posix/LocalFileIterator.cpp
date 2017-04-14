
#include <cppfs/posix/LocalFileIterator.h>

#include <dirent.h>
#include <sys/stat.h>

#include <cppfs/posix/LocalFileSystem.h>


namespace cppfs
{


LocalFileIterator::LocalFileIterator(std::shared_ptr<LocalFileSystem> fs, const std::string & path)
: LocalFileIterator(fs, std::string(path))
{
}

LocalFileIterator::LocalFileIterator(std::shared_ptr<LocalFileSystem> fs, std::string && path)
: m_fs(fs)
, m_path(path)
, m_dir(nullptr)
, m_entry(nullptr)
, m_index(-1)
{
    // Open directory
    m_dir = opendir(m_path.c_str());

    // Read first directory entry
    readNextEntry();
}

LocalFileIterator::~LocalFileIterator()
{
    if (m_dir)
    {
        closedir(m_dir);
    }
}

AbstractFileIteratorBackend * LocalFileIterator::clone() const
{
    auto * twin = new LocalFileIterator(m_fs, m_path);

    while (twin->m_index < m_index)
    {
        twin->readNextEntry();
    }

    return twin;
}

AbstractFileSystem * LocalFileIterator::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

bool LocalFileIterator::valid() const
{
    return (m_dir != nullptr && m_entry != nullptr);
}

std::string LocalFileIterator::path() const
{
    return m_path;
}

int LocalFileIterator::index() const
{
    return m_index;
}

std::string LocalFileIterator::name() const
{
    // Check directory and entry handle
    if (!m_dir || !m_entry)
    {
        return "";
    }

    // Return filename of current item but it's a bad idea to construct an std::string from a nullptr
    return m_entry->d_name ? std::string(m_entry->d_name) : std::string();
}

void LocalFileIterator::next()
{
    readNextEntry();
}

void LocalFileIterator::readNextEntry()
{
    // Check directory handle
    if (!m_dir) return;

    // Read next entry
    m_entry = readdir(m_dir);
    m_index++;
    if (!m_entry || !m_entry->d_name) return;

    // Omit '.' and '..'
    std::string name = m_entry->d_name;
    while (m_entry && m_entry->d_name && (name == ".." || name == "."))
    {
        m_entry = readdir(m_dir);

        name = m_entry && m_entry->d_name ? std::string(m_entry->d_name) : std::string();
    }
}


} // namespace cppfs
