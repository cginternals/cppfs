
#pragma once


namespace cppfs
{


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


} // namespace cppfs
