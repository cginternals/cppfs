
#include <cppfs/windows/LocalFileIterator.h>

#include <Windows.h>

#include <cppfs/FilePath.h>
#include <cppfs/windows/LocalFileSystem.h>


namespace cppfs
{


LocalFileIterator::LocalFileIterator(std::shared_ptr<LocalFileSystem> fs, const std::string & path)
: m_fs(fs)
, m_path(path)
, m_index(-1)
, m_findHandle(nullptr)
, m_findData(nullptr)
{
	// Create find data
	m_findData = static_cast<void *>(new WIN32_FIND_DATA);

	// Read first directory entry
	readNextEntry();
}

LocalFileIterator::~LocalFileIterator()
{
	// Close search
	if (m_findHandle)
	{
		FindClose(m_findHandle);
	}

	// Destroy find data
	delete static_cast<WIN32_FIND_DATA *>(m_findData);
}

std::unique_ptr<AbstractFileIteratorBackend> LocalFileIterator::clone() const
{
    auto * twin = new LocalFileIterator(m_fs, m_path);

    while (twin->m_index < m_index)
    {
        twin->readNextEntry();
    }

    return std::unique_ptr<AbstractFileIteratorBackend>(twin);
}

AbstractFileSystem * LocalFileIterator::fs() const
{
    return static_cast<AbstractFileSystem *>(m_fs.get());
}

bool LocalFileIterator::valid() const
{
    return (m_findHandle != nullptr);
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
    if (!m_findHandle)
    {
        return "";
    }

    // Return filename of current item
	return std::string(static_cast<WIN32_FIND_DATA *>(m_findData)->cFileName);
}

void LocalFileIterator::next()
{
    readNextEntry();
}

void LocalFileIterator::readNextEntry()
{
	std::string filename;

	do
	{
		// Check find handle
		if (!m_findHandle)
		{
			// Open directory
			std::string query = FilePath(m_path).fullPath() + "/*";
			m_findHandle = FindFirstFileA(query.c_str(), static_cast<WIN32_FIND_DATA *>(m_findData));

			// Abort if directory could not be opened
			if (m_findHandle == INVALID_HANDLE_VALUE)
			{
				m_findHandle = nullptr;
				return;
			}
		}

		else {
			// Read next entry
			if (!FindNextFile(m_findHandle, static_cast<WIN32_FIND_DATA *>(m_findData)))
			{
				// No more files, close
				FindClose(m_findHandle);
				m_findHandle = nullptr;
				return;
			}
		}

		// Advance index
		m_index++;

		// Get filename
		filename = std::string(static_cast<WIN32_FIND_DATA *>(m_findData)->cFileName);
	} while (filename == ".." || filename == ".");
}


} // namespace cppfs
