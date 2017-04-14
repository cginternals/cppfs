
#include <cppfs/Change.h>


namespace cppfs
{


Change::Change()
: m_operation(Change::None)
, m_path("")
{
}

Change::Change(Operation operation, const std::string & path)
: m_operation(operation)
, m_path(path)
{
}

Change::Change(Operation operation, std::string && path)
: m_operation(operation)
, m_path(std::move(path))
{
}

Change::Change(const Change & change)
: m_operation(change.m_operation)
, m_path(change.m_path)
{
}

Change::Change(Change && change)
: m_operation(std::move(change.m_operation))
, m_path(std::move(change.m_path))
{
}

Change::~Change()
{
}

Change & Change::operator=(const Change & change)
{
    m_operation = change.m_operation;
    m_path      = change.m_path;

    return *this;
}

Change & Change::operator=(Change && change)
{
    m_operation = std::move(change.m_operation);
    m_path      = std::move(change.m_path);

    return *this;
}

std::string Change::toString() const
{
    switch (m_operation)
    {
        case Change::CopyFile:   return "CP "    + m_path;
        case Change::CopyDir:    return "CPDIR " + m_path;
        case Change::RemoveFile: return "RM "    + m_path;
        case Change::RemoveDir:  return "RMDIR " + m_path;
        default:                 return "NOOP";
    }
}

Change::Operation Change::operation() const
{
    return m_operation;
}

const std::string & Change::path() const
{
    return m_path;
}


} // namespace cppfs
