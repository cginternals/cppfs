
#include <cppfs/Diff.h>

#include <iostream>


namespace cppfs
{


Diff::Diff()
{
}

Diff::~Diff()
{
}

void Diff::clear()
{
    m_changes.clear();
}

const std::vector<Change> & Diff::changes() const
{
    return m_changes;
}

void Diff::add(const Change & change)
{
    m_changes.push_back(change);
}

void Diff::add(Change && change)
{
    m_changes.push_back(change);
}

void Diff::add(Change::Operation operation, const std::string & path)
{
    m_changes.emplace_back(operation, path);
}

void Diff::add(Change::Operation operation, std::string && path)
{
    m_changes.emplace_back(operation, path);
}

void Diff::print(std::ostream & stream)
{
    for (size_t i = 0; i < m_changes.size(); i++)
    {
        stream << m_changes[i].toString() << std::endl;
    }
}

void Diff::print()
{
    print(std::cout);
}


} // namespace cppfs
