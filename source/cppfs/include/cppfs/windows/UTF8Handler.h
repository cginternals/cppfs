
#pragma once


#include <memory>

#include <cppfs/AbstractFileSystem.h>

#if defined(_UNICODE)
    #define UTF8Convert_UTF8toW(s) UTF8Handler::utf8_to_wstring(s)
    #define UTF8Convert_WtoUTF8(s) UTF8Handler::wstring_to_utf8(s)
#else
    #define UTF8Convert_UTF8toW(s) s
    #define UTF8Convert_WtoUTF8(s) s
#endif

namespace cppfs
{
    /**
    *  @brief
    *    Windows specific UTF-8 string conversions
    */
    namespace UTF8Handler
    {
        std::wstring utf8_to_wstring(const std::string& utf8_str);
        std::string wstring_to_utf8(const std::wstring& wstr);
    } // namespace UTF8Handler

} // namespace cppfs
