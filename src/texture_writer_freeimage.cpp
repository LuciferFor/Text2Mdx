#include "texture_writer.h"

#include <FreeImage.h>

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace text2mdx {

namespace {

class FreeImageRuntime {
public:
    FreeImageRuntime() {
        FreeImage_Initialise(TRUE);
    }

    ~FreeImageRuntime() {
        FreeImage_DeInitialise();
    }
};

FREE_IMAGE_FORMAT toFreeImageFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::Tga:
        return FIF_TARGA;
    case TextureFormat::Png:
        return FIF_PNG;
    case TextureFormat::Blp:
        return FIF_UNKNOWN;
    }
    return FIF_UNKNOWN;
}

} // namespace

bool saveWithFreeImage(const Image& image, const std::filesystem::path& path, TextureFormat format) {
    const FREE_IMAGE_FORMAT freeFormat = toFreeImageFormat(format);
    if (freeFormat == FIF_UNKNOWN) {
        return false;
    }

    static FreeImageRuntime runtime;

    FIBITMAP* bitmap = FreeImage_Allocate(image.width, image.height, 32);
    if (!bitmap) {
        throw std::runtime_error("FreeImage_Allocate failed.");
    }

    for (int y = 0; y < image.height; ++y) {
        BYTE* dst = FreeImage_GetScanLine(bitmap, image.height - 1 - y);
        const std::uint8_t* src = image.bgra.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width) * 4;
        std::copy_n(src, static_cast<std::size_t>(image.width) * 4, dst);
    }

    const int flags = format == TextureFormat::Png ? PNG_Z_BEST_COMPRESSION : TARGA_DEFAULT;
    const BOOL ok = FreeImage_SaveU(freeFormat, bitmap, path.c_str(), flags);
    FreeImage_Unload(bitmap);
    if (!ok) {
        throw std::runtime_error("FreeImage failed to save: " + path.u8string());
    }
    return true;
}

} // namespace text2mdx
