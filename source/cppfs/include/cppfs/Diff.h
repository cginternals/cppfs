
#pragma once


#include <vector>
#include <iosfwd>

#include <cppfs/Change.h>


namespace cppfs
{


/**
*  @brief
*    Representation of changes to a directory tree
*/
class CPPFS_API Diff
{
public:
    /**
    *  @brief
    *    Constructor
    */
    Diff();

    /**
    *  @brief
    *    Destructor
    */
    ~Diff();

    /**
    *  @brief
    *    Clear data
    */
    void clear();

    /**
    *  @brief
    *    Get changes
    *
    *  @return
    *    List of changes
    */
    const std::vector<Change> & changes() const;

    /**
    *  @brief
    *    Add change
    *
    *  @param[in] change
    *    Change
    */
    void add(const Change & change);

    /**
    *  @brief
    *    Add change
    *
    *  @param[in] change
    *    Change
    */
    void add(Change && change);

    /**
    *  @brief
    *    Add change
    *
    *  @param[in] operation
    *    Operation type
    *  @param[in] path
    *    Path on which the operation takes place
    */
    void add(Change::Operation operation, const std::string & path);

    /**
    *  @brief
    *    Add change
    *
    *  @param[in] operation
    *    Operation type
    *  @param[in] path
    *    Path on which the operation takes place
    */
    void add(Change::Operation operation, std::string && path);

    /**
    *  @brief
    *    Print changes to stream
    *
    *  @param[in] stream
    *    The output stream
    */
    void print(std::ostream & stream);

    /**
    *  @brief
    *    Print changes to console
    */
    void print();


protected:
    std::vector<Change> m_changes; ///< List of changes
};


} // namespace cppfs
