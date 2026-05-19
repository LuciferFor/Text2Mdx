#include "image.h"

#include "utf.h"

#include <stdexcept>

namespace text2mdx {

std::wstring textureFormatExtension(TextureFormat format) {
    switch (format) {
    case TextureFormat::Blp:
        return L".blp";
    case TextureFormat::Tga:
        return L".tga";
    case TextureFormat::Png:
        return L".png";
    }

    return L".blp";
}

TextureFormat parseTextureFormat(const std::wstring& value) {
    const std::wstring lower = asciiLower(value);
    if (lower == L"blp" || lower == L".blp") {
        return TextureFormat::Blp;
    }
    if (lower == L"tga" || lower == L".tga") {
        return TextureFormat::Tga;
    }
    if (lower == L"png" || lower == L".png") {
        return TextureFormat::Png;
    }

    throw std::runtime_error("Unsupported texture format. Expected blp, tga, or png.");
}

} // namespace text2mdx
