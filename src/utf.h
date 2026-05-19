#pragma once

#include <string>

namespace text2mdx {

std::string wideToUtf8(const std::wstring& value);
std::wstring utf8ToWide(const std::string& value);
std::wstring asciiLower(std::wstring value);

} // namespace text2mdx
