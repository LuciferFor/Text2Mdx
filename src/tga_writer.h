#pragma once

#include "image.h"

#include <filesystem>

namespace text2mdx {

void writeTga(const Image& image, const std::filesystem::path& path);

} // namespace text2mdx
