#pragma once

#include "image.h"

#include <filesystem>

namespace text2mdx {

void writeBlp1PaletteAlpha(const Image& image, const std::filesystem::path& path);

} // namespace text2mdx
