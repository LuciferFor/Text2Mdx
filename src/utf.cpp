#include "utf.h"

#include <algorithm>
#include <stdexcept>
#include <windows.h>

namespace text2mdx {

std::string wideToUtf8(const std::wstring& value) {
    if (value.empty()) {
        return {};
    }

    const int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
    if (bytes <= 0) {
        throw std::runtime_error("WideCharToMultiByte failed.");
    }

    std::string result(static_cast<std::size_t>(bytes), '\0');
    const int written = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), bytes, nullptr, nullptr);
    if (written != bytes) {
        throw std::runtime_error("WideCharToMultiByte wrote an unexpected size.");
    }

    return result;
}

std::wstring utf8ToWide(const std::string& value) {
    if (value.empty()) {
        return {};
    }

    const int chars = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), nullptr, 0);
    if (chars <= 0) {
        throw std::runtime_error("MultiByteToWideChar failed.");
    }

    std::wstring result(static_cast<std::size_t>(chars), L'\0');
    const int written = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, value.data(), static_cast<int>(value.size()), result.data(), chars);
    if (written != chars) {
        throw std::runtime_error("MultiByteToWideChar wrote an unexpected size.");
    }

    return result;
}

std::wstring asciiLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(), [](wchar_t c) {
        if (c >= L'A' && c <= L'Z') {
            return static_cast<wchar_t>(c - L'A' + L'a');
        }
        return c;
    });
    return value;
}

} // namespace text2mdx
