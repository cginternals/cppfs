
#include <gmock/gmock.h>

#include <cppfs/FilePath.h>


using namespace cppfs;


class FilePath_test : public testing::Test
{
public:
    FilePath_test()
    {
    }
};


TEST_F(FilePath_test, originalPath)
{
    const auto unixPath = "/home/user/path/to/file.ext";
    const auto winPath = "C:\\User\\Path\\To\\File.ext";

    auto unixFilePath = FilePath(unixPath);
    auto winFilePath = FilePath(winPath);

    EXPECT_EQ(unixPath, unixFilePath.originalPath());
    EXPECT_EQ(winPath, winFilePath.originalPath());

    unixFilePath.setPath("/home/user/path/");
    EXPECT_EQ("/home/user/path/", unixFilePath.originalPath());
}

TEST_F(FilePath_test, path)
{
    const auto unixPath = "/home/user/path/to/file.ext";
    const auto winPath = "C:\\User\\Path\\To\\File.ext";

    auto unixFilePath = FilePath(unixPath);
    auto winFilePath = FilePath(winPath);

    EXPECT_EQ(unixPath, unixFilePath.path());
    EXPECT_EQ("C:/User/Path/To/File.ext", winFilePath.path());

    unixFilePath.setPath("/home/user/path/");
    EXPECT_EQ("/home/user/path/", unixFilePath.path());
    EXPECT_EQ("/home/user/path", unixFilePath.fullPath());
}

TEST_F(FilePath_test, setPath)
{
    auto filePath = FilePath("/home/user/path/to/file.ext");
    filePath.setPath("/another/path/");

    EXPECT_EQ("/another/path/", filePath.originalPath());
    EXPECT_EQ("/another/path/", filePath.path());
    EXPECT_EQ("/another/path", filePath.fullPath());
}

TEST_F(FilePath_test, baseName)
{
    auto unixFilePath = FilePath("/home/user/path/to/file.ext");
    auto winFilePath = FilePath("C:\\User\\Path\\To\\File.ext");

    EXPECT_EQ("file", unixFilePath.baseName());
    EXPECT_EQ("File", winFilePath.baseName());

    unixFilePath.setPath("/home/user/path/to/.file.ext");
    EXPECT_EQ(".file", unixFilePath.baseName());

    unixFilePath.setPath("/home/user/path/to/directory");
    EXPECT_EQ("directory", unixFilePath.baseName());

    unixFilePath.setPath("/home/user/path/to/directory/");
    EXPECT_EQ("directory", unixFilePath.baseName());

    unixFilePath.setPath("/home/user/path/to/directory.ext/");
    EXPECT_EQ("directory", unixFilePath.baseName());
}

TEST_F(FilePath_test, fileName)
{
    auto unixFilePath = FilePath("/home/user/path/to/file.ext");
    auto winFilePath = FilePath("C:\\User\\Path\\To\\File.ext");

    EXPECT_EQ("file.ext", unixFilePath.fileName());
    EXPECT_EQ("File.ext", winFilePath.fileName());

    unixFilePath.setPath("/home/user/path/to/.file.ext");
    EXPECT_EQ(".file.ext", unixFilePath.fileName());

    unixFilePath.setPath("/home/user/path/to/directory");
    EXPECT_EQ("directory", unixFilePath.fileName());

    unixFilePath.setPath("/home/user/path/to/directory/");
    EXPECT_EQ("directory", unixFilePath.fileName());

    unixFilePath.setPath("/home/user/path/to/directory.ext/");
    EXPECT_EQ("directory.ext", unixFilePath.fileName());
}

TEST_F(FilePath_test, extension)
{
    auto unixFilePath = FilePath("/home/user/path/to/file.ext");
    auto winFilePath = FilePath("C:\\User\\Path\\To\\File.ext");

    EXPECT_EQ(".ext", unixFilePath.extension());
    EXPECT_EQ(".ext", winFilePath.extension());

    unixFilePath.setPath("/home/user/path/to/directory/");
    EXPECT_EQ("", unixFilePath.extension());

    unixFilePath.setPath("/home/user/path/to/file");
    EXPECT_EQ("", unixFilePath.extension());
}

TEST_F(FilePath_test, directoryPath)
{
    auto unixFilePath = FilePath("/home/user/path/to/file.ext");
    auto winFilePath = FilePath("C:\\User\\Path\\To\\File.ext");

    EXPECT_EQ("/home/user/path/to/", unixFilePath.directoryPath());
    EXPECT_EQ("C:/User/Path/To/", winFilePath.directoryPath());

    unixFilePath.setPath("/home/user/path/to/directory");
    EXPECT_EQ("/home/user/path/to/", unixFilePath.directoryPath());

    unixFilePath.setPath("/home/user/path/to/directory/");
    EXPECT_EQ("/home/user/path/to/", unixFilePath.directoryPath());
}

TEST_F(FilePath_test, driveLetter)
{
    auto unixFilePath = FilePath("/home/user/path/to/file.ext");
    auto winFilePath = FilePath("C:\\User\\Path\\To\\File.ext");

    EXPECT_EQ("", unixFilePath.driveLetter());
    EXPECT_EQ("C:", winFilePath.driveLetter());
}

TEST_F(FilePath_test, directoryPathPlusFileName)
{
    const auto unixPath = "/home/user/path/to/file.ext";
    const auto winPath = "C:\\User\\Path\\To\\File.ext";

    auto unixFilePath = FilePath(unixPath);
    auto winFilePath = FilePath(winPath);

    EXPECT_EQ(unixPath, unixFilePath.directoryPath() + unixFilePath.fileName());
    EXPECT_EQ("C:/User/Path/To/File.ext", winFilePath.directoryPath() + winFilePath.fileName());
}
