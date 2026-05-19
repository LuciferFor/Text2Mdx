#pragma once

#include "image.h"
#include "style.h"

#include <string>

namespace text2mdx {

Image renderText(const std::wstring& text, const TextStyle& style);

} // namespace text2mdx
