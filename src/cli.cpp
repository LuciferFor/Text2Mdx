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
    if (argc <= 1) {
        options.help = true;
        return options;
    }

    for (int i = 1; i < argc; ++i) {
        const wchar_t* arg = argv[i];
        if (isOption(arg, L"--help") || isOption(arg, L"-h") || isOption(arg, L"/?") || isOption(arg, L"help")) {
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
    constexpr const char* reset = "\x1b[0m";
    constexpr const char* bold = "\x1b[1m";
    constexpr const char* dim = "\x1b[2m";
    constexpr const char* cyan = "\x1b[36m";
    constexpr const char* green = "\x1b[32m";
    constexpr const char* yellow = "\x1b[33m";
    constexpr const char* magenta = "\x1b[35m";

    std::cout
        << bold << cyan << "Text2Mdx" << reset << " - Warcraft III 飘字/技能名 MDX 模型生成器\n"
        << dim << "把中文文本渲染成透明贴图，再写入一个 billboarded 平面 .mdx 模型。\n\n" << reset

        << bold << yellow << "用法" << reset << "\n"
        << "  " << green << "Text2Mdx.exe --text \"暴击!\" --style crit --out \".\\out\\crit.mdx\"" << reset << "\n"
        << "  " << green << "Text2Mdx.exe --reference \".\\templates\\crit_example.mdx\" --text \"烈火剑法\" --out \".\\out\\fire.mdx\"" << reset << "\n"
        << "  " << green << "Text2Mdx.exe --text \"机器\" --style machine_blue --texture-format png --preview-png \".\\out\\machine.png\" --out \".\\out\\machine.mdx\"" << reset << "\n"
        << "  " << green << "Text2Mdx.exe --list-styles" << reset << "\n\n"

        << bold << yellow << "必填参数" << reset << "\n"
        << "  " << cyan << "--text <文本>" << reset << "                 要生成的文字，例如 \"暴击!\"、\"烈火剑法\"、\"无名功法\"。\n"
        << "  " << cyan << "--out <路径>" << reset << "                  输出 .mdx 文件路径，例如 \".\\out\\crit.mdx\"。\n\n"

        << bold << yellow << "样式参数" << reset << "\n"
        << "  " << cyan << "--style <名称>" << reset << "                样式预设名。默认: damage_red。\n"
        << "                                 内置: damage_red, crit, fire_skill, machine_blue, gold_name。\n"
        << "  " << cyan << "--style-config <路径>" << reset << "         样式 JSON 文件。默认: presets/styles.json。\n"
        << "  " << cyan << "--list-styles" << reset << "                列出当前样式配置里的所有样式名。\n\n"

        << bold << yellow << "模板换字" << reset << "\n"
        << "  " << cyan << "--reference <模型.mdx>" << reset << "        参考模型换文字。\n"
        << "                                 有 .text2mdx.json 时复用字体/描边/发光等配方。\n"
        << "                                 没有 sidecar 时只记录外部来源，并生成一个静态可见的文字平面模型。\n"
        << "                                 外部 MDX 默认把贴图放在 .mdx 同目录，方便本地模型编辑器预览。\n"
        << "                                 别名: --template, --from-template。\n"
        << "                                 可额外传 --style 或 --texture-format 覆盖模板配置。\n\n"

        << bold << yellow << "贴图参数" << reset << "\n"
        << "  " << cyan << "--texture-format blp|tga|png" << reset << "  贴图格式。默认: blp。\n"
        << "                                 blp 适合 Warcraft III；png 方便预览或调试。\n"
        << "  " << cyan << "--texture-path <路径>" << reset << "         写进 MDX 的贴图路径。默认: Textures\\<out-stem>.<ext>。\n"
        << "  " << cyan << "--preview-png <路径>" << reset << "          额外保存一张 PNG 预览图，不影响 MDX 使用的主贴图。\n\n"

        << bold << yellow << "输出文件" << reset << "\n"
        << "  " << magenta << "<name>.mdx" << reset << "                 生成的 Warcraft III 模型。\n"
        << "  " << magenta << "Textures\\<name>.blp" << reset << "         默认主贴图。\n"
        << "  " << magenta << "<name>.text2mdx.json" << reset << "       模板配方文件，用于以后 --reference 换文本。\n\n"

        << bold << yellow << "说明" << reset << "\n"
        << "  - 外部 MDX 没有 .text2mdx.json 时，工具不会复制它的骨骼动画壳；文字样式会使用 --style 或默认 damage_red。\n"
        << "  - 模型是面向镜头的平面贴图，不是真 3D 字体网格。\n"
        << "  - 不带任何参数、" << cyan << "-h" << reset << "、" << cyan << "--help" << reset << "、" << cyan << "/?" << reset << " 都会显示本帮助。\n";
}

} // namespace text2mdx
