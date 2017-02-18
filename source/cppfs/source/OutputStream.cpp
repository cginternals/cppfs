
#include <cppfs/OutputStream.h>


namespace cppfs
{


OutputStream::OutputStream(std::streambuf * sb)
: std::ostream(sb)
, m_sb(sb)
{
}

OutputStream::~OutputStream()
{
    m_sb->pubsync();
    delete m_sb;
}


} // namespace cppfs
