#pragma once

#include <filesystem>
#include <string>

namespace text2mdx {

struct MdxOptions {
    std::filesystem::path outPath;
    std::string modelName = "Text2Mdx";
    std::string texturePath;
    int contentWidth = 1;
    int contentHeight = 1;
    float uMax = 1.0f;
    float vMax = 1.0f;
    float planeHeight = 120.0f;
};

void writeMdxBillboard(const MdxOptions& options);

} // namespace text2mdx
