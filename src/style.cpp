#include "style.h"

#include "json_value.h"
#include "utf.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace text2mdx {

namespace {

std::string readUtf8File(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open style config: " + path.u8string());
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

const JsonValue* maybe(const JsonValue::Object& object, const std::string& key) {
    const auto it = object.find(key);
    return it == object.end() ? nullptr : &it->second;
}

std::wstring stringValue(const JsonValue::Object& object, const std::string& key, const std::wstring& fallback) {
    if (const JsonValue* value = maybe(object, key); value && value->isString()) {
        return utf8ToWide(value->asString());
    }
    return fallback;
}

float numberValue(const JsonValue::Object& object, const std::string& key, float fallback) {
    if (const JsonValue* value = maybe(object, key); value && value->isNumber()) {
        return static_cast<float>(value->asNumber());
    }
    return fallback;
}

int intValue(const JsonValue::Object& object, const std::string& key, int fallback) {
    if (const JsonValue* value = maybe(object, key); value && value->isNumber()) {
        return static_cast<int>(value->asNumber());
    }
    return fallback;
}

bool boolValue(const JsonValue::Object& object, const std::string& key, bool fallback) {
    if (const JsonValue* value = maybe(object, key); value && value->isBool()) {
        return value->asBool();
    }
    return fallback;
}

Color colorValue(const JsonValue::Object& object, const std::string& key, Color fallback) {
    if (const JsonValue* value = maybe(object, key); value && value->isString()) {
        return parseColor(utf8ToWide(value->asString()));
    }
    return fallback;
}

TextStyle parseStyle(const std::string& id, const JsonValue& value) {
    const JsonValue::Object& object = value.asObject();

    TextStyle style;
    style.id = id;
    style.fontFamily = stringValue(object, "fontFamily", style.fontFamily);
    style.fontSize = numberValue(object, "fontSize", style.fontSize);
    style.fontWeight = intValue(object, "fontWeight", style.fontWeight);
    style.italic = boolValue(object, "italic", style.italic);
    style.skewDegrees = numberValue(object, "skewDegrees", style.skewDegrees);
    style.padding = numberValue(object, "padding", style.padding);
    style.planeHeight = numberValue(object, "planeHeight", style.planeHeight);
    style.fillTop = colorValue(object, "fillTop", style.fillTop);
    style.fillBottom = colorValue(object, "fillBottom", style.fillBottom);
    style.outlineColor = colorValue(object, "outlineColor", style.outlineColor);
    style.outlineWidth = numberValue(object, "outlineWidth", style.outlineWidth);
    style.shadowColor = colorValue(object, "shadowColor", style.shadowColor);
    style.shadowOffsetX = numberValue(object, "shadowOffsetX", style.shadowOffsetX);
    style.shadowOffsetY = numberValue(object, "shadowOffsetY", style.shadowOffsetY);
    style.glowColor = colorValue(object, "glowColor", style.glowColor);
    style.glowWidth = numberValue(object, "glowWidth", style.glowWidth);
    return style;
}

} // namespace

StyleLibrary StyleLibrary::load(const std::filesystem::path& path) {
    const JsonValue root = parseJson(readUtf8File(path));
    const JsonValue* stylesValue = root.find("styles");
    if (!stylesValue) {
        stylesValue = &root;
    }

    StyleLibrary library;
    for (const auto& [name, value] : stylesValue->asObject()) {
        library.styles_.emplace(name, parseStyle(name, value));
    }

    if (library.styles_.empty()) {
        throw std::runtime_error("Style config contains no styles.");
    }
    return library;
}

const TextStyle& StyleLibrary::get(const std::string& id) const {
    const auto it = styles_.find(id);
    if (it == styles_.end()) {
        throw std::runtime_error("Unknown style: " + id);
    }
    return it->second;
}

std::vector<std::string> StyleLibrary::names() const {
    std::vector<std::string> result;
    for (const auto& [name, _] : styles_) {
        result.push_back(name);
    }
    return result;
}

} // namespace text2mdx
