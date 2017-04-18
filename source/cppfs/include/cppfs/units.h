
#pragma once


namespace cppfs
{


#define SIZE_B(v)  (v)
#define SIZE_KB(v) (v*1024)
#define SIZE_MB(v) (v*1024*1024)
#define SIZE_GB(v) (v*1024*1024*1024)
#define SIZE_TB(v) (v*1024*1024*1024*1024)


/*
// Unfortunately, user defined literals are not supported in VS 2013.
// We should use this when we decide to drop support for VS 2013.

inline unsigned long long operator""_B(unsigned long long value)
{
    return value;
}

inline unsigned long long operator""_kB(unsigned long long value)
{
    return value * 1024;
}

inline unsigned long long operator""_MB(unsigned long long value)
{
    return value * 1024 * 1024;
}

inline unsigned long long operator""_GB(unsigned long long value)
{
    return value * 1024 * 1024 * 1024;
}

inline unsigned long long operator""_TB(unsigned long long value)
{
    return value * 1024 * 1024 * 1024 * 1024;
}
*/


} // namespace cppfs
