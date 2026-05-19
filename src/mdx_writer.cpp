#include "mdx_writer.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace text2mdx {

namespace {

class Writer {
public:
    void tag(const char (&value)[5]) {
        bytes_.insert(bytes_.end(), value, value + 4);
    }

    void u8(std::uint8_t value) {
        bytes_.push_back(value);
    }

    void u16(std::uint16_t value) {
        bytes_.push_back(static_cast<std::uint8_t>(value & 0xFF));
        bytes_.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
    }

    void u32(std::uint32_t value) {
        bytes_.push_back(static_cast<std::uint8_t>(value & 0xFF));
        bytes_.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFF));
        bytes_.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFF));
        bytes_.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFF));
    }

    void i32(std::int32_t value) {
        u32(static_cast<std::uint32_t>(value));
    }

    void f32(float value) {
        std::uint32_t bits = 0;
        static_assert(sizeof(bits) == sizeof(value), "float size mismatch");
        std::memcpy(&bits, &value, sizeof(bits));
        u32(bits);
    }

    void fixedString(const std::string& value, std::size_t size) {
        const std::size_t count = std::min(value.size(), size);
        bytes_.insert(bytes_.end(), value.begin(), value.begin() + static_cast<std::ptrdiff_t>(count));
        bytes_.insert(bytes_.end(), size - count, 0);
    }

    std::size_t position() const {
        return bytes_.size();
    }

    void patchU32(std::size_t position, std::uint32_t value) {
        bytes_[position] = static_cast<std::uint8_t>(value & 0xFF);
        bytes_[position + 1] = static_cast<std::uint8_t>((value >> 8) & 0xFF);
        bytes_[position + 2] = static_cast<std::uint8_t>((value >> 16) & 0xFF);
        bytes_[position + 3] = static_cast<std::uint8_t>((value >> 24) & 0xFF);
    }

    const std::vector<std::uint8_t>& bytes() const {
        return bytes_;
    }

private:
    std::vector<std::uint8_t> bytes_;
};

struct Chunk {
    Writer& writer;
    std::size_t sizePosition = 0;
    std::size_t dataStart = 0;

    Chunk(Writer& writer, const char (&name)[5]) : writer(writer) {
        writer.tag(name);
        sizePosition = writer.position();
        writer.u32(0);
        dataStart = writer.position();
    }

    ~Chunk() {
        const std::size_t size = writer.position() - dataStart;
        writer.patchU32(sizePosition, static_cast<std::uint32_t>(size));
    }
};

void writeExtent(Writer& writer, float halfWidth, float halfHeight) {
    const float radius = std::sqrt(halfWidth * halfWidth + halfHeight * halfHeight);
    writer.f32(radius);
    writer.f32(-halfWidth);
    writer.f32(0.0f);
    writer.f32(-halfHeight);
    writer.f32(halfWidth);
    writer.f32(0.0f);
    writer.f32(halfHeight);
}

void writeSequence(Writer& writer, float halfWidth, float halfHeight) {
    Chunk chunk(writer, "SEQS");
    writer.fixedString("Stand", 80);
    writer.u32(0);
    writer.u32(1000);
    writer.f32(0.0f);
    writer.u32(0);
    writer.f32(0.0f);
    writer.u32(0);
    writeExtent(writer, halfWidth, halfHeight);
}

void writeTexture(Writer& writer, const std::string& texturePath) {
    Chunk chunk(writer, "TEXS");
    writer.u32(0);
    writer.fixedString(texturePath, 260);
    writer.u32(0);
}

void writeMaterial(Writer& writer) {
    Chunk chunk(writer, "MTLS");
    writer.u32(48); // Material byte length for version 800 with one layer.
    writer.i32(0);
    writer.u32(0);
    writer.tag("LAYS");
    writer.u32(1);
    writer.u32(28); // Layer byte length for version 800.
    writer.u32(2);  // Blend.
    writer.u32(0x1 | 0x10 | 0x20 | 0x40 | 0x80); // Unshaded, two-sided, unfogged, no depth test/set.
    writer.i32(0);
    writer.i32(-1);
    writer.u32(0);
    writer.f32(1.0f);
}

void writeGeoset(Writer& writer, float halfWidth, float halfHeight, float uMax, float vMax) {
    Chunk chunk(writer, "GEOS");
    const std::size_t objectStart = writer.position();
    writer.u32(0); // Patched geoset object size.

    const float vertices[12] = {
        -halfWidth, 0.0f, halfHeight,
        halfWidth, 0.0f, halfHeight,
        halfWidth, 0.0f, -halfHeight,
        -halfWidth, 0.0f, -halfHeight,
    };
    writer.tag("VRTX");
    writer.u32(4);
    for (float value : vertices) {
        writer.f32(value);
    }

    writer.tag("NRMS");
    writer.u32(4);
    for (int i = 0; i < 4; ++i) {
        writer.f32(0.0f);
        writer.f32(-1.0f);
        writer.f32(0.0f);
    }

    writer.tag("PTYP");
    writer.u32(1);
    writer.u32(4);

    writer.tag("PCNT");
    writer.u32(1);
    writer.u32(6);

    const std::uint16_t faces[6] = {0, 1, 2, 0, 2, 3};
    writer.tag("PVTX");
    writer.u32(6);
    for (std::uint16_t face : faces) {
        writer.u16(face);
    }

    writer.tag("GNDX");
    writer.u32(4);
    for (int i = 0; i < 4; ++i) {
        writer.u8(0);
    }

    writer.tag("MTGC");
    writer.u32(1);
    writer.u32(1);

    writer.tag("MATS");
    writer.u32(1);
    writer.u32(0);

    writer.u32(0); // Material ID.
    writer.u32(0); // Selection group.
    writer.u32(0); // Selection flags.
    writeExtent(writer, halfWidth, halfHeight);
    writer.u32(1);
    writeExtent(writer, halfWidth, halfHeight);

    writer.tag("UVAS");
    writer.u32(1);
    writer.tag("UVBS");
    writer.u32(4);
    const float uv[8] = {
        0.0f, 0.0f,
        uMax, 0.0f,
        uMax, vMax,
        0.0f, vMax,
    };
    for (float value : uv) {
        writer.f32(value);
    }

    writer.patchU32(objectStart, static_cast<std::uint32_t>(writer.position() - objectStart));
}

void writeBone(Writer& writer) {
    Chunk chunk(writer, "BONE");
    writer.u32(96); // Generic object byte length.
    writer.fixedString("Billboard_Bone", 80);
    writer.i32(0);
    writer.i32(-1);
    writer.u32(0x100 | 0x8); // Bone flag plus Billboarded.
    writer.i32(0);
    writer.i32(-1);
}

void writePivot(Writer& writer) {
    Chunk chunk(writer, "PIVT");
    writer.f32(0.0f);
    writer.f32(0.0f);
    writer.f32(0.0f);
}

} // namespace

void writeMdxBillboard(const MdxOptions& options) {
    if (options.texturePath.empty()) {
        throw std::runtime_error("MDX texture path is empty.");
    }

    const float planeHeight = std::max(1.0f, options.planeHeight);
    const float aspect = static_cast<float>(std::max(1, options.contentWidth)) / static_cast<float>(std::max(1, options.contentHeight));
    const float planeWidth = planeHeight * aspect;
    const float halfWidth = planeWidth * 0.5f;
    const float halfHeight = planeHeight * 0.5f;

    Writer writer;
    writer.tag("MDLX");

    {
        Chunk chunk(writer, "VERS");
        writer.u32(800);
    }
    {
        Chunk chunk(writer, "MODL");
        writer.fixedString(options.modelName, 80);
        writer.fixedString("", 260);
        writeExtent(writer, halfWidth, halfHeight);
        writer.u32(150);
    }

    writeSequence(writer, halfWidth, halfHeight);
    writeTexture(writer, options.texturePath);
    writeMaterial(writer);
    writeGeoset(writer, halfWidth, halfHeight, options.uMax, options.vMax);
    writeBone(writer);
    writePivot(writer);

    if (!options.outPath.parent_path().empty()) {
        std::filesystem::create_directories(options.outPath.parent_path());
    }
    std::ofstream file(options.outPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open MDX output: " + options.outPath.u8string());
    }
    const std::vector<std::uint8_t>& bytes = writer.bytes();
    file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

} // namespace text2mdx
