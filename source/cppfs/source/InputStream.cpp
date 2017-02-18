
#include <cppfs/InputStream.h>


namespace cppfs
{


InputStream::InputStream(std::streambuf * sb)
: std::istream(sb)
, m_sb(sb)
{
}

InputStream::~InputStream()
{
    delete m_sb;
}


} // namespace cppfs
