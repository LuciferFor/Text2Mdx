#pragma once

#include "image.h"

#include <filesystem>
#include <string>

namespace text2mdx {

struct Options {
    std::wstring text;
    std::string style = "damage_red";
    std::filesystem::path styleConfig = L"presets/styles.json";
    std::filesystem::path referencePath;
    std::filesystem::path outPath;
    TextureFormat textureFormat = TextureFormat::Blp;
    std::wstring texturePath;
    std::filesystem::path previewPng;
    bool styleSpecified = false;
    bool styleConfigSpecified = false;
    bool textureFormatSpecified = false;
    bool listStyles = false;
    bool help = false;
};

Options parseOptions(int argc, wchar_t** argv);
void printUsage();

} // namespace text2mdx
