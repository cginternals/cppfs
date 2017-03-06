
#include <stdlib.h>

#include <cppfs/system.h>


namespace cppfs
{
namespace system
{


std::string homeDir()
{
#if defined(SYSTEM_WINDOWS)
	return std::string(getenv("HOMEPATH"));
#else
	return std::string(getenv("HOME"));
#endif
}

std::string configDir(const std::string & application)
{
#if defined(SYSTEM_WINDOWS)
    return std::string(getenv("APPDATA")) + "\\" + application;
#elif defined(SYSTEM_DARWIN)
    return std::string(getenv("HOME")) + "/Library/Preferences/" + application;
#else
    return std::string(getenv("HOME")) + "/.config/" + application;
#endif
}


} // namespace system
} // namespace cppfs
