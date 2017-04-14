
#include <stdlib.h>

#include <cppfs/system.h>


namespace
{

std::string obtainHomeDir()
{
#if defined(SYSTEM_WINDOWS)
    return std::string(getenv("HOMEPATH"));
#else
    return std::string(getenv("HOME"));
#endif
}

std::string obtainConfigDir()
{
#if defined(SYSTEM_WINDOWS)
    return std::string(getenv("APPDATA")) + "\\";
#elif defined(SYSTEM_DARWIN)
    return std::string(getenv("HOME")) + "/Library/Preferences/";
#else
    return std::string(getenv("HOME")) + "/.config/";
#endif
}

} // namespace


namespace cppfs
{


namespace system
{


const std::string & homeDir()
{
    static const std::string dir = obtainHomeDir();

    return dir;
}

std::string configDir(const std::string & application)
{
    static const std::string dir = obtainConfigDir();

    return dir + application;
}


} // namespace system


} // namespace cppfs
