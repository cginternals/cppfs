
#pragma once


namespace cppfs
{


inline unsigned long long size_b(unsigned long long value)
{
    return value;
}

inline unsigned long long size_kb(unsigned long long value)
{
    return value * 1024;
}

inline unsigned long long size_mb(unsigned long long value)
{
    return value * 1024 * 1024;
}

inline unsigned long long size_gb(unsigned long long value)
{
    return value * 1024 * 1024 * 1024;
}

inline unsigned long long size_tb(unsigned long long value)
{
    return value * 1024 * 1024 * 1024 * 1024;
}


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
