#include "cli.h"
#include "json_value.h"
#include "mdx_writer.h"
#include "style.h"
#include "text_renderer.h"
#include "texture_writer.h"
#include "utf.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <windows.h>

namespace text2mdx {

namespace {

std::filesystem::path exeDirectory() {
    std::wstring buffer(MAX_PATH, L'\0');
    DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    while (size == buffer.size()) {
        buffer.resize(buffer.size() * 2);
        size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    }
    if (size == 0) {
        return std::filesystem::current_path();
    }
    buffer.resize(size);
    return std::filesystem::path(buffer).parent_path();
}

std::filesystem::path resolveStyleConfig(const std::filesystem::path& requested) {
    if (std::filesystem::exists(requested)) {
        return requested;
    }

    const std::filesystem::path besideExe = exeDirectory() / requested;
    if (std::filesystem::exists(besideExe)) {
        return besideExe;
    }

    const std::filesystem::path installed = exeDirectory().parent_path() / requested;
    if (std::filesystem::exists(installed)) {
        return installed;
    }

    return requested;
}

std::filesystem::path templateInfoPath(std::filesystem::path mdxPath) {
    return mdxPath.replace_extension(L".text2mdx.json");
}

std::string textureFormatName(TextureFormat format) {
    switch (format) {
    case TextureFormat::Blp:
        return "blp";
    case TextureFormat::Tga:
        return "tga";
    case TextureFormat::Png:
        return "png";
    }
    return "blp";
}

std::string jsonEscape(const std::string& value) {
    std::string escaped;
    for (const unsigned char c : value) {
        switch (c) {
        case '"':
            escaped += "\\\"";
            break;
        case '\\':
            escaped += "\\\\";
            break;
        case '\b':
            escaped += "\\b";
            break;
        case '\f':
            escaped += "\\f";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            if (c < 0x20) {
                static constexpr char hex[] = "0123456789abcdef";
                escaped += "\\u00";
                escaped.push_back(hex[(c >> 4) & 0x0F]);
                escaped.push_back(hex[c & 0x0F]);
            } else {
                escaped.push_back(static_cast<char>(c));
            }
            break;
        }
    }
    return escaped;
}

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open template sidecar: " + path.u8string());
    }
    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

std::filesystem::path pathFromUtf8(const std::string& value) {
    return std::filesystem::path(utf8ToWide(value));
}

void applyReferenceTemplate(Options& options) {
    if (options.referencePath.empty()) {
        return;
    }

    const std::filesystem::path infoPath = templateInfoPath(options.referencePath);
    if (!std::filesystem::exists(infoPath)) {
        throw std::runtime_error(
            "Reference model has no Text2Mdx sidecar: " + infoPath.u8string() +
            ". Generate the reference with Text2Mdx first, or use --style directly.");
    }

    const JsonValue root = parseJson(readTextFile(infoPath));
    if (!root.isObject()) {
        throw std::runtime_error("Template sidecar root must be a JSON object: " + infoPath.u8string());
    }

    if (!options.styleSpecified) {
        options.style = root.at("style").asString();
    }
    if (!options.styleConfigSpecified) {
        if (const JsonValue* styleConfig = root.find("style_config")) {
            std::filesystem::path candidate = pathFromUtf8(styleConfig->asString());
            if (!candidate.is_absolute()) {
                const std::filesystem::path besideTemplate = infoPath.parent_path() / candidate;
                if (std::filesystem::exists(besideTemplate)) {
                    candidate = besideTemplate;
                }
            }
            options.styleConfig = candidate;
        }
    }
    if (!options.textureFormatSpecified) {
        if (const JsonValue* textureFormat = root.find("texture_format")) {
            options.textureFormat = parseTextureFormat(utf8ToWide(textureFormat->asString()));
        }
    }
}

void writeTemplateInfo(const Options& options) {
    const std::filesystem::path infoPath = templateInfoPath(options.outPath);
    if (!infoPath.parent_path().empty()) {
        std::filesystem::create_directories(infoPath.parent_path());
    }

    std::ofstream file(infoPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not write template sidecar: " + infoPath.u8string());
    }

    file
        << "{\n"
        << "  \"text2mdx_template_version\": 1,\n"
        << "  \"style\": \"" << jsonEscape(options.style) << "\",\n"
        << "  \"style_config\": \"" << jsonEscape(options.styleConfig.u8string()) << "\",\n"
        << "  \"texture_format\": \"" << textureFormatName(options.textureFormat) << "\"\n"
        << "}\n";
}

std::wstring normalizeMdxPath(std::wstring value) {
    for (wchar_t& c : value) {
        if (c == L'/') {
            c = L'\\';
        }
    }
    return value;
}

std::wstring defaultTexturePath(const std::filesystem::path& outPath, TextureFormat format) {
    return (std::filesystem::path(L"Textures") / (outPath.stem().wstring() + textureFormatExtension(format))).wstring();
}

std::filesystem::path textureDiskPath(const std::filesystem::path& outPath, const std::wstring& mdxTexturePath) {
    const std::filesystem::path path(mdxTexturePath);
    if (path.is_absolute()) {
        return path;
    }
    return outPath.parent_path() / path;
}

int run(int argc, wchar_t** argv) {
    SetConsoleOutputCP(CP_UTF8);
    Options options = parseOptions(argc, argv);
    if (options.help) {
        printUsage();
        return 0;
    }
    if (!options.listStyles) {
        applyReferenceTemplate(options);
    }

    const std::filesystem::path styleConfig = resolveStyleConfig(options.styleConfig);
    const StyleLibrary styles = StyleLibrary::load(styleConfig);

    if (options.listStyles) {
        for (const std::string& name : styles.names()) {
            std::cout << name << '\n';
        }
        return 0;
    }

    const TextStyle& style = styles.get(options.style);
    const Image image = renderText(options.text, style);

    const std::wstring mdxTexturePathW = normalizeMdxPath(options.texturePath.empty()
        ? defaultTexturePath(options.outPath, options.textureFormat)
        : options.texturePath);
    const std::filesystem::path diskTexturePath = textureDiskPath(options.outPath, mdxTexturePathW);

    const TextureWriteResult texture = writeTexture(TextureWriteRequest{image, options.textureFormat, diskTexturePath});

    if (!options.previewPng.empty()) {
        writeTexture(TextureWriteRequest{image, TextureFormat::Png, options.previewPng});
    }

    MdxOptions mdx;
    mdx.outPath = options.outPath;
    mdx.modelName = wideToUtf8(options.outPath.stem().wstring());
    mdx.texturePath = wideToUtf8(mdxTexturePathW);
    mdx.contentWidth = image.width;
    mdx.contentHeight = image.height;
    mdx.uMax = texture.uMax;
    mdx.vMax = texture.vMax;
    mdx.planeHeight = style.planeHeight;
    writeMdxBillboard(mdx);
    writeTemplateInfo(options);

    std::wcout << L"Wrote " << options.outPath.wstring() << L"\n";
    std::wcout << L"Wrote " << diskTexturePath.wstring() << L"\n";
    std::wcout << L"Wrote " << templateInfoPath(options.outPath).wstring() << L"\n";
    return 0;
}

} // namespace

} // namespace text2mdx

int wmain(int argc, wchar_t** argv) {
    try {
        return text2mdx::run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Text2Mdx error: " << e.what() << '\n';
        return 1;
    }
}
