#include "texture_writer.h"

#include "blp_writer.h"
#include "tga_writer.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace text2mdx {

namespace {

int nextPowerOfTwo(int value) {
    int power = 1;
    while (power < value) {
        power <<= 1;
    }
    return power;
}

Image padToPowerOfTwo(const Image& image) {
    const int width = nextPowerOfTwo(image.width);
    const int height = nextPowerOfTwo(image.height);
    if (width == image.width && height == image.height) {
        return image;
    }

    Image padded;
    padded.width = width;
    padded.height = height;
    padded.bgra.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4, 0);

    for (int y = 0; y < image.height; ++y) {
        const std::size_t src = static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width) * 4;
        const std::size_t dst = static_cast<std::size_t>(y) * static_cast<std::size_t>(width) * 4;
        std::copy_n(image.bgra.data() + src, static_cast<std::size_t>(image.width) * 4, padded.bgra.data() + dst);
    }

    return padded;
}

} // namespace

TextureWriteResult writeTexture(const TextureWriteRequest& request) {
    if (request.image.empty()) {
        throw std::runtime_error("Texture image is empty.");
    }

    if (!request.diskPath.parent_path().empty()) {
        std::filesystem::create_directories(request.diskPath.parent_path());
    }

    TextureWriteResult result;
    result.storedWidth = request.image.width;
    result.storedHeight = request.image.height;

    Image image = request.image;
    if (request.format == TextureFormat::Blp) {
        image = padToPowerOfTwo(request.image);
        result.storedWidth = image.width;
        result.storedHeight = image.height;
        result.uMax = static_cast<float>(request.image.width) / static_cast<float>(image.width);
        result.vMax = static_cast<float>(request.image.height) / static_cast<float>(image.height);
    }

#ifdef TEXT2MDX_HAS_RESIL
    if (saveWithResil(image, request.diskPath, request.format)) {
        return result;
    }
#endif

    if (request.format == TextureFormat::Blp) {
        writeBlp1PaletteAlpha(image, request.diskPath);
        return result;
    }

#ifdef TEXT2MDX_HAS_FREEIMAGE
    if (saveWithFreeImage(image, request.diskPath, request.format)) {
        return result;
    }
#endif

    if (request.format == TextureFormat::Tga) {
        writeTga(image, request.diskPath);
        return result;
    }

    throw std::runtime_error("PNG output needs ResIL/IL or FreeImage. Use --texture-format blp/tga or enable an image backend.");
}

} // namespace text2mdx
