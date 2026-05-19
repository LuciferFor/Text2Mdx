#pragma once

#include "color.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace text2mdx {

struct TextStyle {
    std::string id;
    std::wstring fontFamily = L"Microsoft YaHei UI";
    float fontSize = 88.0f;
    int fontWeight = 900;
    bool italic = true;
    float skewDegrees = -8.0f;
    float padding = 28.0f;
    float planeHeight = 120.0f;

    Color fillTop = parseColor(L"#fff06a");
    Color fillBottom = parseColor(L"#d71900");
    Color outlineColor = parseColor(L"#160000");
    float outlineWidth = 5.0f;
    Color shadowColor = parseColor(L"#000000a0");
    float shadowOffsetX = 4.0f;
    float shadowOffsetY = 5.0f;
    Color glowColor = parseColor(L"#ff4c00a0");
    float glowWidth = 4.0f;
};

class StyleLibrary {
public:
    static StyleLibrary load(const std::filesystem::path& path);

    const TextStyle& get(const std::string& id) const;
    std::vector<std::string> names() const;

private:
    std::map<std::string, TextStyle> styles_;
};

} // namespace text2mdx
