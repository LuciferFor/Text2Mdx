#include "tga_writer.h"

#include <array>
#include <fstream>
#include <stdexcept>

namespace text2mdx {

void writeTga(const Image& image, const std::filesystem::path& path) {
    if (image.empty()) {
        throw std::runtime_error("Cannot write empty TGA image.");
    }
    if (image.width > 65535 || image.height > 65535) {
        throw std::runtime_error("TGA dimensions exceed 16-bit limits.");
    }

    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open TGA output: " + path.u8string());
    }

    std::array<std::uint8_t, 18> header{};
    header[2] = 2; // Uncompressed true-color.
    header[12] = static_cast<std::uint8_t>(image.width & 0xFF);
    header[13] = static_cast<std::uint8_t>((image.width >> 8) & 0xFF);
    header[14] = static_cast<std::uint8_t>(image.height & 0xFF);
    header[15] = static_cast<std::uint8_t>((image.height >> 8) & 0xFF);
    header[16] = 32;
    header[17] = 0x28; // 8 alpha bits, top-left origin.
    file.write(reinterpret_cast<const char*>(header.data()), static_cast<std::streamsize>(header.size()));
    file.write(reinterpret_cast<const char*>(image.bgra.data()), static_cast<std::streamsize>(image.bgra.size()));
}

} // namespace text2mdx
