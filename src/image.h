#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace text2mdx {

struct Image {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> bgra;

    bool empty() const {
        return width <= 0 || height <= 0 || bgra.empty();
    }
};

enum class TextureFormat {
    Blp,
    Tga,
    Png,
};

struct TextureWriteRequest {
    Image image;
    TextureFormat format = TextureFormat::Blp;
    std::filesystem::path diskPath;
};

struct TextureWriteResult {
    int storedWidth = 0;
    int storedHeight = 0;
    float uMax = 1.0f;
    float vMax = 1.0f;
};

std::wstring textureFormatExtension(TextureFormat format);
TextureFormat parseTextureFormat(const std::wstring& value);

} // namespace text2mdx
