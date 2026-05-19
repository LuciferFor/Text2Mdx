#pragma once

#include "image.h"

namespace text2mdx {

TextureWriteResult writeTexture(const TextureWriteRequest& request);

#ifdef TEXT2MDX_HAS_RESIL
bool saveWithResil(const Image& image, const std::filesystem::path& path, TextureFormat format);
#endif

#ifdef TEXT2MDX_HAS_FREEIMAGE
bool saveWithFreeImage(const Image& image, const std::filesystem::path& path, TextureFormat format);
#endif

} // namespace text2mdx
