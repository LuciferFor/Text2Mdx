#pragma once

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace text2mdx {

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
    float a = 1.0f;
};

inline int hexNibble(wchar_t c) {
    if (c >= L'0' && c <= L'9') {
        return static_cast<int>(c - L'0');
    }
    if (c >= L'a' && c <= L'f') {
        return 10 + static_cast<int>(c - L'a');
    }
    if (c >= L'A' && c <= L'F') {
        return 10 + static_cast<int>(c - L'A');
    }
    throw std::runtime_error("Invalid hex color.");
}

inline int hexByte(const std::wstring& s, std::size_t index) {
    return hexNibble(s[index]) * 16 + hexNibble(s[index + 1]);
}

inline Color parseColor(const std::wstring& s) {
    if (s.empty() || s[0] != L'#' || (s.size() != 7 && s.size() != 9)) {
        throw std::runtime_error("Color must be #RRGGBB or #RRGGBBAA.");
    }

    Color color;
    color.r = static_cast<float>(hexByte(s, 1)) / 255.0f;
    color.g = static_cast<float>(hexByte(s, 3)) / 255.0f;
    color.b = static_cast<float>(hexByte(s, 5)) / 255.0f;
    color.a = s.size() == 9 ? static_cast<float>(hexByte(s, 7)) / 255.0f : 1.0f;
    return color;
}

inline std::uint8_t toByte(float value) {
    value = std::clamp(value, 0.0f, 1.0f);
    return static_cast<std::uint8_t>(value * 255.0f + 0.5f);
}

} // namespace text2mdx
