#include "blp_writer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace text2mdx {

namespace {

struct PaletteColor {
    std::uint8_t b = 0;
    std::uint8_t g = 0;
    std::uint8_t r = 0;
    std::uint32_t count = 0;
};

void writeU32(std::ofstream& file, std::uint32_t value) {
    const std::uint8_t bytes[4] = {
        static_cast<std::uint8_t>(value & 0xFF),
        static_cast<std::uint8_t>((value >> 8) & 0xFF),
        static_cast<std::uint8_t>((value >> 16) & 0xFF),
        static_cast<std::uint8_t>((value >> 24) & 0xFF),
    };
    file.write(reinterpret_cast<const char*>(bytes), 4);
}

std::uint32_t quantKey(std::uint8_t b, std::uint8_t g, std::uint8_t r) {
    const std::uint32_t rb = r >> 3;
    const std::uint32_t gb = g >> 2;
    const std::uint32_t bb = b >> 3;
    return (rb << 11) | (gb << 5) | bb;
}

PaletteColor colorFromKey(std::uint32_t key, std::uint32_t count) {
    const std::uint8_t r5 = static_cast<std::uint8_t>((key >> 11) & 0x1F);
    const std::uint8_t g6 = static_cast<std::uint8_t>((key >> 5) & 0x3F);
    const std::uint8_t b5 = static_cast<std::uint8_t>(key & 0x1F);

    PaletteColor color;
    color.r = static_cast<std::uint8_t>((r5 << 3) | (r5 >> 2));
    color.g = static_cast<std::uint8_t>((g6 << 2) | (g6 >> 4));
    color.b = static_cast<std::uint8_t>((b5 << 3) | (b5 >> 2));
    color.count = count;
    return color;
}

std::vector<PaletteColor> buildPalette(const Image& image) {
    std::unordered_map<std::uint32_t, std::uint32_t> histogram;
    histogram.reserve(512);

    for (std::size_t i = 0; i < image.bgra.size(); i += 4) {
        const std::uint8_t alpha = image.bgra[i + 3];
        if (alpha < 8) {
            continue;
        }
        histogram[quantKey(image.bgra[i], image.bgra[i + 1], image.bgra[i + 2])] += 1;
    }

    std::vector<PaletteColor> palette;
    palette.reserve(std::min<std::size_t>(histogram.size(), 256));
    for (const auto& [key, count] : histogram) {
        palette.push_back(colorFromKey(key, count));
    }

    std::sort(palette.begin(), palette.end(), [](const PaletteColor& a, const PaletteColor& b) {
        return a.count > b.count;
    });

    if (palette.empty()) {
        palette.push_back(PaletteColor{255, 255, 255, 1});
    }
    if (palette.size() > 256) {
        palette.resize(256);
    }
    while (palette.size() < 256) {
        palette.push_back(palette.back());
    }
    return palette;
}

std::uint8_t nearestPaletteIndex(const std::vector<PaletteColor>& palette, std::uint8_t b, std::uint8_t g, std::uint8_t r) {
    int bestIndex = 0;
    int bestDistance = std::numeric_limits<int>::max();

    for (int i = 0; i < static_cast<int>(palette.size()); ++i) {
        const int db = static_cast<int>(b) - palette[static_cast<std::size_t>(i)].b;
        const int dg = static_cast<int>(g) - palette[static_cast<std::size_t>(i)].g;
        const int dr = static_cast<int>(r) - palette[static_cast<std::size_t>(i)].r;
        const int distance = db * db + dg * dg + dr * dr;
        if (distance < bestDistance) {
            bestDistance = distance;
            bestIndex = i;
            if (distance == 0) {
                break;
            }
        }
    }

    return static_cast<std::uint8_t>(bestIndex);
}

} // namespace

void writeBlp1PaletteAlpha(const Image& image, const std::filesystem::path& path) {
    if (image.empty()) {
        throw std::runtime_error("Cannot write empty BLP image.");
    }

    const std::uint32_t pixelCount = static_cast<std::uint32_t>(image.width * image.height);
    const std::uint32_t headerSize = 156;
    const std::uint32_t paletteSize = 1024;
    const std::uint32_t mipOffset = headerSize + paletteSize;
    const std::uint32_t mipSize = pixelCount * 2; // 8-bit palette indices + 8-bit alpha.

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open BLP output: " + path.u8string());
    }

    writeU32(file, 0x31504C42); // BLP1.
    writeU32(file, 1);          // Direct/palettized content.
    writeU32(file, 8);          // Alpha bits.
    writeU32(file, static_cast<std::uint32_t>(image.width));
    writeU32(file, static_cast<std::uint32_t>(image.height));
    writeU32(file, 5);          // Common Warcraft III direct type.
    writeU32(file, 0);          // No mipmaps.

    for (int i = 0; i < 16; ++i) {
        writeU32(file, i == 0 ? mipOffset : 0);
    }
    for (int i = 0; i < 16; ++i) {
        writeU32(file, i == 0 ? mipSize : 0);
    }

    const std::vector<PaletteColor> palette = buildPalette(image);
    for (const PaletteColor& color : palette) {
        const std::uint8_t bgra[4] = {color.b, color.g, color.r, 255};
        file.write(reinterpret_cast<const char*>(bgra), 4);
    }

    std::vector<std::uint8_t> indices(pixelCount);
    std::vector<std::uint8_t> alpha(pixelCount);

    for (std::uint32_t i = 0; i < pixelCount; ++i) {
        const std::size_t src = static_cast<std::size_t>(i) * 4;
        indices[i] = nearestPaletteIndex(palette, image.bgra[src], image.bgra[src + 1], image.bgra[src + 2]);
        alpha[i] = image.bgra[src + 3];
    }

    file.write(reinterpret_cast<const char*>(indices.data()), static_cast<std::streamsize>(indices.size()));
    file.write(reinterpret_cast<const char*>(alpha.data()), static_cast<std::streamsize>(alpha.size()));
}

} // namespace text2mdx
