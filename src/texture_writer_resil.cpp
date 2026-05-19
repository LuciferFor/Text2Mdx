#include "texture_writer.h"

#include "utf.h"

#include <IL/il2.h>

#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>

namespace text2mdx {

namespace {

class ResilRuntime {
public:
    ResilRuntime() {
        il2Init();
        il2Enable(IL_FILE_OVERWRITE);
    }
};

struct ImageDeleter {
    void operator()(ILimage* image) const {
        if (image != nullptr) {
            il2DeleteImage(image);
        }
    }
};

ILFileFormat toIlFormat(TextureFormat format) {
    switch (format) {
    case TextureFormat::Blp:
        return IL_BLP;
    case TextureFormat::Tga:
        return IL_TGA;
    case TextureFormat::Png:
        return IL_PNG;
    }
    return IL_TYPE_UNKNOWN;
}

std::string ilStringToUtf8(ILconst_string value) {
    if (value == nullptr || value[0] == 0) {
        return {};
    }
#ifdef _UNICODE
    return wideToUtf8(std::wstring(value));
#else
    return std::string(value);
#endif
}

struct ResilError {
    ILenum code = IL_NO_ERROR;
    std::string message;
};

ResilError readResilError(ILimage* image) {
    ResilError error;
    error.message = ilStringToUtf8(il2GetErrorString(image));
    error.code = il2GetError(image);
    return error;
}

std::string resilErrorText(const ResilError& error) {
    const std::string& message = error.message;
    if (!message.empty()) {
        return message + " (code " + std::to_string(error.code) + ")";
    }
    return "error code " + std::to_string(error.code);
}

bool canFallbackFromSaveError(ILenum code) {
    return code == IL_INVALID_EXTENSION || code == IL_FORMAT_NOT_SUPPORTED || code == IL_INVALID_ENUM;
}

} // namespace

bool saveWithResil(const Image& image, const std::filesystem::path& path, TextureFormat format) {
    static ResilRuntime runtime;

    const ILFileFormat ilFormat = toIlFormat(format);
    if (ilFormat == IL_TYPE_UNKNOWN) {
        return false;
    }

    std::unique_ptr<ILimage, ImageDeleter> ilImage(il2GenImage());
    if (!ilImage) {
        throw std::runtime_error("ResIL failed to allocate image.");
    }

    const ILboolean okImage = il2TexImage(
        ilImage.get(),
        static_cast<ILuint>(image.width),
        static_cast<ILuint>(image.height),
        1,
        IL_BGRA,
        IL_UNSIGNED_BYTE,
        image.bgra.data());
    if (!okImage) {
        throw std::runtime_error("ResIL failed to create image: " + resilErrorText(readResilError(ilImage.get())));
    }

    const ILboolean okConvert = il2ConvertImage(ilImage.get(), IL_RGBA, IL_UNSIGNED_BYTE);
    if (!okConvert) {
        throw std::runtime_error("ResIL failed to convert image: " + resilErrorText(readResilError(ilImage.get())));
    }

    const std::string utf8Path = path.u8string();
    ILboolean okSave = il2Save(ilImage.get(), ilFormat, path.c_str());
    if (!okSave) {
        const ResilError error = readResilError(ilImage.get());
        if (canFallbackFromSaveError(error.code)) {
            // Some ResIL builds expose load support for a format without a saver.
            okSave = il2SaveImage(ilImage.get(), path.c_str());
            if (okSave) {
                return true;
            }
            (void)readResilError(ilImage.get());
            std::error_code removeError;
            std::filesystem::remove(path, removeError);
            return false;
        }
        throw std::runtime_error("ResIL failed to save " + utf8Path + ": " + resilErrorText(error));
    }
    return true;
}

} // namespace text2mdx
