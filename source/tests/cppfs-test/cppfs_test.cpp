
#include <gmock/gmock.h>
#include "../../cppfs/source/SHA1.hpp"


class cppfs_test: public testing::Test
{
public:
};


TEST_F(cppfs_test, CheckSomeResults)
{
    // [TODO] Implement test
    std::stringstream t1("");
    std::stringstream t2("abc");
    EXPECT_EQ(sha1(t1),"da39a3ee5e6b4b0d3255bfef95601890afd80709");
    EXPECT_EQ(sha1(t2),"a9993e364706816aba3e25717850c26c9cd0d89d");
    EXPECT_EQ((unsigned int) 0, 0);
    // ...
}
