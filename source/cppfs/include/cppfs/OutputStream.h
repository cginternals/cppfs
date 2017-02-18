
#pragma once


#include <ostream>

#include <cppfs/cppfs_api.h>


namespace cppfs
{


/**
*  @brief
*    Output stream
*/
class CPPFS_API OutputStream : public std::ostream
{
public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] sb
    *    Stream buffer (must NOT be null!)
    *
    *  @remarks
    *    The output stream takes ownership over the stream buffer
    */
    explicit OutputStream(std::streambuf * sb);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~OutputStream();


protected:
    std::streambuf * m_sb; ///< The associated stream buffer
};


} // namespace cppfs
