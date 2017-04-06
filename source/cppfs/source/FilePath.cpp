
#include <cppfs/FilePath.h>

#include <sstream>
#include <algorithm>
#include <vector>
#include <string>


namespace cppfs
{


FilePath::FilePath()
: m_path("")
, m_pointsToContent(false)
, m_details(false)
{
}

FilePath::FilePath(const FilePath & filePath)
: m_path(filePath.m_path)
, m_pointsToContent(filePath.m_pointsToContent)
, m_details(filePath.m_details)
, m_fullPath(filePath.m_fullPath)
, m_filename(filePath.m_filename)
, m_basename(filePath.m_basename)
, m_extension(filePath.m_extension)
, m_directoryPath(filePath.m_directoryPath)
, m_driveLetter(filePath.m_driveLetter)
, m_absolute(filePath.m_absolute)
{
}

FilePath::FilePath(FilePath && filePath)
: m_path(std::move(filePath.m_path))
, m_pointsToContent(std::move(filePath.m_pointsToContent))
, m_details(std::move(filePath.m_details))
, m_fullPath(std::move(filePath.m_fullPath))
, m_filename(std::move(filePath.m_filename))
, m_basename(std::move(filePath.m_basename))
, m_extension(std::move(filePath.m_extension))
, m_directoryPath(std::move(filePath.m_directoryPath))
, m_driveLetter(std::move(filePath.m_driveLetter))
, m_absolute(std::move(filePath.m_absolute))
{
}

FilePath::FilePath(const std::string & path)
: m_pointsToContent(false)
, m_details(false)
{
    setPath(path);
}

FilePath::FilePath(std::string && path)
: m_pointsToContent(false)
, m_details(false)
{
    setPath(path);
}

FilePath::FilePath(const char * path)
: m_pointsToContent(false)
, m_details(false)
{
    setPath(path);
}

FilePath::~FilePath()
{
}

FilePath & FilePath::operator=(const FilePath & filePath)
{
    m_path            = filePath.m_path;
    m_pointsToContent = filePath.m_pointsToContent;
    m_details         = filePath.m_details;
    m_fullPath        = filePath.m_fullPath;
    m_filename        = filePath.m_filename;
    m_basename        = filePath.m_basename;
    m_extension       = filePath.m_extension;
    m_directoryPath   = filePath.m_directoryPath;
    m_driveLetter     = filePath.m_driveLetter;
    m_absolute        = filePath.m_absolute;

    return *this;
}

FilePath & FilePath::operator=(FilePath && filePath)
{
    m_path            = std::move(filePath.m_path);
    m_pointsToContent = std::move(filePath.m_pointsToContent);
    m_details         = std::move(filePath.m_details);
    m_fullPath        = std::move(filePath.m_fullPath);
    m_filename        = std::move(filePath.m_filename);
    m_basename        = std::move(filePath.m_basename);
    m_extension       = std::move(filePath.m_extension);
    m_directoryPath   = std::move(filePath.m_directoryPath);
    m_driveLetter     = std::move(filePath.m_driveLetter);
    m_absolute        = std::move(filePath.m_absolute);

    return *this;
}

const std::string & FilePath::path() const
{
    return m_path;
}

void FilePath::setPath(const std::string & path)
{
    // Set new path
    m_path = path;

    // Reset state
    m_pointsToContent = false;
    m_details         = false;
    m_fullPath        = "";
    m_filename        = "";
    m_basename        = "";
    m_extension       = "";
    m_directoryPath   = "";
    m_driveLetter     = "";
    m_absolute        = false;

    // Convert path into unified form
    std::replace(m_path.begin(), m_path.end(), '\\', '/');

    // Check if path ends with a delimiter
    auto pos = m_path.find_last_of('/');
    if (pos == m_path.size()-1)
    {
        m_pointsToContent = true;
    }
}

void FilePath::setPath(std::string && path)
{
    // Set new path
    m_path = std::move(path);

    // Reset state
    m_pointsToContent = false;
    m_details         = false;
    m_fullPath        = "";
    m_filename        = "";
    m_basename        = "";
    m_extension       = "";
    m_directoryPath   = "";
    m_driveLetter     = "";
    m_absolute        = false;

    // Convert path into unified form
    std::replace(m_path.begin(), m_path.end(), '\\', '/');

    // Check if path ends with a delimiter
    auto pos = m_path.find_last_of('/');
    if (pos == m_path.size()-1)
    {
        m_pointsToContent = true;
    }
}

std::string FilePath::toNative() const
{
    auto path = m_path;

#if defined(SYSTEM_WINDOWS)
    std::replace(path.begin(), path.end(), '/', '\\');
#endif

    return path;
}

bool FilePath::isEmpty() const
{
    return m_path.empty();
}

bool FilePath::pointsToContent() const
{
    return m_pointsToContent;
}

const std::string & FilePath::fullPath() const
{
    analyze();

    return m_fullPath;
}

const std::string & FilePath::fileName() const
{
    analyze();

    return m_filename;
}

const std::string & FilePath::baseName() const
{
    analyze();

    return m_basename;
}

const std::string & FilePath::extension() const
{
    analyze();

    return m_extension;
}

const std::string & FilePath::directoryPath() const
{
    analyze();

    return m_directoryPath;
}

const std::string & FilePath::driveLetter() const
{
    analyze();

    return m_driveLetter;
}

bool FilePath::isAbsolute() const
{
    analyze();

    return m_absolute;
}

bool FilePath::isRelative() const
{
    analyze();

    return !m_absolute;
}

FilePath FilePath::resolve(const FilePath & path) const
{
    // Return empty path if both paths are empty
    if (path.isEmpty() && isEmpty())
    {
        return FilePath();
    }

    // Return input path if it is an absolute path or this path is empty
    if (path.isAbsolute() || isEmpty())
    {
        return path;
    }

    // Return this path if input path is empty
    if (path.isEmpty())
    {
        return *this;
    }

    // Combine paths
    return FilePath(fullPath() + "/" + path.path());
}

std::string FilePath::resolved() const
{
    // Split path into parts
    std::vector<std::string> parts;
    std::stringstream ss(m_path);

    std::string name;
    while (std::getline(ss, name, '/'))
    {
        parts.push_back(name);
    }

    // Process sub-paths, handling '.' and '..'
    std::vector<std::string> stack;
    size_t removableItems = 0;
    bool absolute = false;

    size_t numParts = parts.size();
    for (size_t i = 0; i < numParts; i++)
    {
        // Get sub-path
        const std::string & path = parts[i];

        // Check if it is the beginning of an absolute path
        if (i == 0 && (path.empty() || (path.length() == 2 && path[1] == ':'))) {
            absolute = true;
        }

        // Ignore '.'
        if (path == ".") {
            continue;
        }

        // Handle '..'
        else if (path == ".." && removableItems > 0)
        {
            stack.pop_back();
            removableItems--;
        }

        // Regular sub-path
        else
        {
            // Add to stack
            stack.push_back(path);

            // Check if sub-path is removable by '..' entries
            if (!(i == 0 && absolute) && path != "..")
            {
                removableItems++;
            }
        }
    }

    // Compose resolved string
    std::string resolved = "";

    for (size_t i = 0; i < stack.size(); i++)
    {
        // Add sub-string
        resolved += stack[i];

        // Add separator, except for the last item (unless it is also the beginning of an absolute path)
        if (i + 1 < stack.size() || (i == 0 && absolute))
            resolved += "/";
    }

    // Return resolved path
    if (resolved.empty()) return ".";
    else                  return resolved;
}

void FilePath::analyze() const
{
    // Abort if already analyzed
    if (m_details)
    {
        return;
    }

    // Reset details
    m_fullPath      = "";
    m_filename      = "";
    m_basename      = "";
    m_extension     = "";
    m_directoryPath = "";
    m_driveLetter   = "";
    m_absolute      = false;

    // Split path into parts
    std::vector<std::string> parts;
    std::stringstream ss(m_path);

    std::string name;
    while (std::getline(ss, name, '/'))
    {
        parts.push_back(name);
    }

    // Fix sub-paths
    size_t numParts = parts.size();

    for (size_t i = 0; i < numParts; i++)
    {
        // Get sub-path
        const std::string & path = parts[i];

        // If this is the first path and it is absolute, ensure '/' at the end
        if (i == 0 && (path.empty() || (path.length() == 2 && path[1] == ':')))
        {
            parts[i] += "/";
            m_absolute = true;
        }

        // For all sub-paths before the filename, ensure '/' at the end
        else if (i < numParts - 1)
        {
            parts[i] += "/";
        }
    }

    // Determine filename
    m_filename = (numParts > 0) ? parts[numParts-1] : "";

    // Determine directory path and full path
    m_directoryPath = "";

    if (numParts > 1)
    {
        // Path has at least two parts
        for (size_t i = 0; i<numParts-1; i++)
        {
            m_directoryPath += parts[i];
        }

        m_fullPath = m_directoryPath + m_filename;
    }

    else if (numParts == 1 && m_absolute)
    {
        // Path has only one part and that is absolute
        m_directoryPath = parts[0];
        m_fullPath      = m_directoryPath;
    }

    else if (numParts == 1)
    {
        // Path has one part that is not absolute
        m_fullPath = m_filename;
    }

    // Determine basename and extension
    size_t pos = m_filename.find_first_of('.', 1);

    if (m_filename == "." || m_filename == ".." || pos == std::string::npos)
    {
        m_basename  = m_filename;
        m_extension = "";
    }

    else
    {
        m_basename  = m_filename.substr(0, pos);
        m_extension = m_filename.substr(pos);
    }

    // Determine drive letter
    pos = m_fullPath.find_first_of(':');

    m_driveLetter = pos == 1 ? m_path.substr(0, pos+1) : "";

    // Set state
    m_details = true;
}


} // namespace cppfs
