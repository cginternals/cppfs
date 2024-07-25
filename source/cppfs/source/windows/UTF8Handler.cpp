
#include <cppfs/windows/UTF8Handler.h>
#include <windows.h>
#include <stdexcept>


namespace cppfs
{
    namespace UTF8Handler
    {
        std::wstring utf8_to_wstring(const std::string& utf8_str) {
            if (utf8_str.empty()) {
                return std::wstring();
            }

            const int wstr_len = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
            if (wstr_len == 0) {
                throw std::runtime_error("Error converting UTF-8 string to UTF-16.");
            }

            std::wstring wstr(wstr_len, L'\0');

            if (MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &wstr[0], wstr_len) == 0) {
                throw std::runtime_error("Error converting UTF-8 string to UTF-16.");
            }
            wstr.resize(static_cast<size_t>(wstr_len) - 1); // -1 because MultiByteToWideChar includes the null terminator in its count

            return wstr;
        }

        std::string wstring_to_utf8(const std::wstring& wstr) {
            if (wstr.empty()) {
                return std::string();
            }

            const int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
            if (utf8_len == 0) {
                throw std::runtime_error("Error converting UTF-16 string to UTF-8.");
            }

            std::string utf8_str(utf8_len, '\0');
            if (WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8_str[0], utf8_len, nullptr, nullptr) == 0) {
                throw std::runtime_error("Error converting UTF-16 string to UTF-8.");
            }
            utf8_str.resize(static_cast<size_t>(utf8_len) - 1); // -1 because WideCharToMultiByte includes the null terminator in its count

            return utf8_str;
        }
    }
} // namespace cppfs
