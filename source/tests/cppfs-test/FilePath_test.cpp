
#include <gmock/gmock.h>

#include <cppfs/FilePath.h>


class FilePath_test: public testing::Test
{
public:
};


TEST_F(FilePath_test, testResolved)
{
    // Taken from documentation of FilePath::resolved
    const auto f1 = cppfs::FilePath("a/b/../c"); // -> "a/c"
    const auto f2 = cppfs::FilePath("../../a"); // -> "../../a"
    const auto f3 = cppfs::FilePath("a/../../b"); // -> "../b"
    const auto f4 = cppfs::FilePath("/a/../b/"); // -> "/b"

    // Taken from https://github.com/cginternals/cppfs/issues/38
    const auto f5 = cppfs::FilePath("/usr/bin/bash"); // "/usr/bin/bash"
    const auto f6 = cppfs::FilePath("/usr/bin/../bin/bash"); // "/usr/bin/bash"
    const auto f7 = cppfs::FilePath("/usr/bin//../bin/bash"); // -> "/usr/bin/bash"

    EXPECT_EQ("a/c", f1.resolved());
    EXPECT_EQ("../../a", f2.resolved());
    EXPECT_EQ("../b", f3.resolved());
    EXPECT_EQ("/b", f4.resolved());
    EXPECT_EQ("/usr/bin/bash", f5.resolved());
    EXPECT_EQ("/usr/bin/bash", f6.resolved());
    EXPECT_EQ("/usr/bin/bash", f7.resolved());
}
