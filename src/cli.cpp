#include "cli.h"

#include "utf.h"

#include <iostream>
#include <stdexcept>

namespace text2mdx {

namespace {

bool isOption(const wchar_t* value, const wchar_t* option) {
    return std::wstring(value) == option;
}

std::wstring requireValue(int& i, int argc, wchar_t** argv, const wchar_t* option) {
    if (i + 1 >= argc) {
        throw std::runtime_error("Missing value after " + wideToUtf8(option));
    }
    ++i;
    return argv[i];
}

} // namespace

Options parseOptions(int argc, wchar_t** argv) {
    Options options;

    for (int i = 1; i < argc; ++i) {
        const wchar_t* arg = argv[i];
        if (isOption(arg, L"--help") || isOption(arg, L"-h")) {
            options.help = true;
        } else if (isOption(arg, L"--list-styles")) {
            options.listStyles = true;
        } else if (isOption(arg, L"--text")) {
            options.text = requireValue(i, argc, argv, arg);
        } else if (isOption(arg, L"--style")) {
            options.style = wideToUtf8(requireValue(i, argc, argv, arg));
            options.styleSpecified = true;
        } else if (isOption(arg, L"--style-config")) {
            options.styleConfig = requireValue(i, argc, argv, arg);
            options.styleConfigSpecified = true;
        } else if (isOption(arg, L"--reference") || isOption(arg, L"--template") || isOption(arg, L"--from-template")) {
            options.referencePath = requireValue(i, argc, argv, arg);
        } else if (isOption(arg, L"--out")) {
            options.outPath = requireValue(i, argc, argv, arg);
        } else if (isOption(arg, L"--texture-format")) {
            options.textureFormat = parseTextureFormat(requireValue(i, argc, argv, arg));
            options.textureFormatSpecified = true;
        } else if (isOption(arg, L"--texture-path")) {
            options.texturePath = requireValue(i, argc, argv, arg);
        } else if (isOption(arg, L"--preview-png")) {
            options.previewPng = requireValue(i, argc, argv, arg);
        } else {
            throw std::runtime_error("Unknown option: " + wideToUtf8(arg));
        }
    }

    if (!options.help && !options.listStyles) {
        if (options.text.empty()) {
            throw std::runtime_error("--text is required.");
        }
        if (options.outPath.empty()) {
            throw std::runtime_error("--out is required.");
        }
    }

    return options;
}

void printUsage() {
    std::cout
        << "Text2Mdx - generate Warcraft III billboard text MDX models\n\n"
        << "Usage:\n"
        << "  Text2Mdx.exe --text \"暴击!\" --style crit --out \".\\out\\crit.mdx\"\n"
        << "  Text2Mdx.exe --reference \".\\out\\crit.mdx\" --text \"烈火剑法\" --out \".\\out\\fire.mdx\"\n"
        << "  Text2Mdx.exe --text \"烈火剑法\" --style fire_skill --texture-format tga --out \".\\out\\fire.mdx\"\n"
        << "  Text2Mdx.exe --list-styles\n\n"
        << "Options:\n"
        << "  --text <text>                 Text to render.\n"
        << "  --style <name>                Style preset name. Default: damage_red.\n"
        << "  --style-config <path>         JSON style file. Default: presets/styles.json.\n"
        << "  --reference <mdx>             Reuse a Text2Mdx sidecar as the style template.\n"
        << "  --out <path>                  Output .mdx path.\n"
        << "  --texture-format blp|tga|png  Texture format. Default: blp.\n"
        << "  --texture-path <path>         MDX texture path. Default: Textures\\<out-stem>.<ext>.\n"
        << "  --preview-png <path>          Optional PNG preview.\n"
        << "  --list-styles                 Print style names.\n";
}

} // namespace text2mdx
