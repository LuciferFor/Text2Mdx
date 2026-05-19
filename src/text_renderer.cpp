#include "text_renderer.h"

#include <algorithm>
#include <cmath>
#include <d2d1.h>
#include <dwrite.h>
#include <stdexcept>
#include <wincodec.h>
#include <wrl/client.h>

namespace text2mdx {

namespace {

using Microsoft::WRL::ComPtr;

void check(HRESULT hr, const char* message) {
    if (FAILED(hr)) {
        throw std::runtime_error(message);
    }
}

D2D1_COLOR_F d2dColor(Color color) {
    return D2D1::ColorF(color.r, color.g, color.b, color.a);
}

class ComInit {
public:
    ComInit() {
        hr_ = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr_) && hr_ != RPC_E_CHANGED_MODE) {
            check(hr_, "CoInitializeEx failed.");
        }
    }

    ~ComInit() {
        if (SUCCEEDED(hr_)) {
            CoUninitialize();
        }
    }

private:
    HRESULT hr_ = S_OK;
};

class GlyphRenderer final : public IDWriteTextRenderer {
public:
    GlyphRenderer(ID2D1Factory* factory, ID2D1RenderTarget* target, ID2D1Brush* fill, ID2D1Brush* stroke, float strokeWidth)
        : factory_(factory), target_(target), fill_(fill), stroke_(stroke), strokeWidth_(strokeWidth) {}

    IFACEMETHOD(QueryInterface)(REFIID iid, void** object) override {
        if (!object) {
            return E_POINTER;
        }
        if (iid == __uuidof(IUnknown) || iid == __uuidof(IDWritePixelSnapping) || iid == __uuidof(IDWriteTextRenderer)) {
            *object = this;
            AddRef();
            return S_OK;
        }
        *object = nullptr;
        return E_NOINTERFACE;
    }

    IFACEMETHOD_(ULONG, AddRef)() override {
        return InterlockedIncrement(&refCount_);
    }

    IFACEMETHOD_(ULONG, Release)() override {
        const ULONG count = InterlockedDecrement(&refCount_);
        if (count == 0) {
            delete this;
        }
        return count;
    }

    IFACEMETHOD(IsPixelSnappingDisabled)(void*, BOOL* disabled) override {
        *disabled = FALSE;
        return S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(void*, DWRITE_MATRIX* transform) override {
        D2D1_MATRIX_3X2_F matrix;
        target_->GetTransform(&matrix);
        transform->m11 = matrix._11;
        transform->m12 = matrix._12;
        transform->m21 = matrix._21;
        transform->m22 = matrix._22;
        transform->dx = matrix._31;
        transform->dy = matrix._32;
        return S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(void*, FLOAT* pixelsPerDip) override {
        float dpiX = 96.0f;
        float dpiY = 96.0f;
        target_->GetDpi(&dpiX, &dpiY);
        *pixelsPerDip = dpiX / 96.0f;
        return S_OK;
    }

    IFACEMETHOD(DrawGlyphRun)(
        void*,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE,
        const DWRITE_GLYPH_RUN* glyphRun,
        const DWRITE_GLYPH_RUN_DESCRIPTION*,
        IUnknown*) override {
        ComPtr<ID2D1PathGeometry> geometry;
        check(factory_->CreatePathGeometry(&geometry), "CreatePathGeometry failed.");

        ComPtr<ID2D1GeometrySink> sink;
        check(geometry->Open(&sink), "Open geometry sink failed.");
        check(glyphRun->fontFace->GetGlyphRunOutline(
                  glyphRun->fontEmSize,
                  glyphRun->glyphIndices,
                  glyphRun->glyphAdvances,
                  glyphRun->glyphOffsets,
                  glyphRun->glyphCount,
                  glyphRun->isSideways,
                  (glyphRun->bidiLevel % 2) != 0,
                  sink.Get()),
              "GetGlyphRunOutline failed.");
        check(sink->Close(), "Close geometry sink failed.");

        D2D1_MATRIX_3X2_F oldTransform;
        target_->GetTransform(&oldTransform);
        target_->SetTransform(D2D1::Matrix3x2F::Translation(baselineOriginX, baselineOriginY) * oldTransform);
        if (fill_) {
            target_->FillGeometry(geometry.Get(), fill_);
        }
        if (stroke_ && strokeWidth_ > 0.0f) {
            target_->DrawGeometry(geometry.Get(), stroke_, strokeWidth_);
        }
        target_->SetTransform(oldTransform);
        return S_OK;
    }

    IFACEMETHOD(DrawUnderline)(void*, FLOAT x, FLOAT y, const DWRITE_UNDERLINE* underline, IUnknown*) override {
        if (!fill_) {
            return S_OK;
        }
        const D2D1_RECT_F rect = D2D1::RectF(x, y + underline->offset, x + underline->width, y + underline->offset + underline->thickness);
        target_->FillRectangle(rect, fill_);
        return S_OK;
    }

    IFACEMETHOD(DrawStrikethrough)(void*, FLOAT x, FLOAT y, const DWRITE_STRIKETHROUGH* strike, IUnknown*) override {
        if (!fill_) {
            return S_OK;
        }
        const D2D1_RECT_F rect = D2D1::RectF(x, y + strike->offset, x + strike->width, y + strike->offset + strike->thickness);
        target_->FillRectangle(rect, fill_);
        return S_OK;
    }

    IFACEMETHOD(DrawInlineObject)(void*, FLOAT, FLOAT, IDWriteInlineObject*, BOOL, BOOL, IUnknown*) override {
        return S_OK;
    }

private:
    volatile ULONG refCount_ = 1;
    ID2D1Factory* factory_ = nullptr;
    ID2D1RenderTarget* target_ = nullptr;
    ID2D1Brush* fill_ = nullptr;
    ID2D1Brush* stroke_ = nullptr;
    float strokeWidth_ = 0.0f;
};

void drawLayout(
    ID2D1Factory* factory,
    ID2D1RenderTarget* target,
    IDWriteTextLayout* layout,
    ID2D1Brush* fill,
    ID2D1Brush* stroke,
    float strokeWidth,
    float x,
    float y) {
    GlyphRenderer* renderer = new GlyphRenderer(factory, target, fill, stroke, strokeWidth);
    const HRESULT hr = layout->Draw(nullptr, renderer, x, y);
    renderer->Release();
    check(hr, "Draw text layout failed.");
}

} // namespace

Image renderText(const std::wstring& text, const TextStyle& style) {
    if (text.empty()) {
        throw std::runtime_error("Cannot render empty text.");
    }

    ComInit com;

    ComPtr<ID2D1Factory> d2dFactory;
    check(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf()), "D2D1CreateFactory failed.");

    ComPtr<IDWriteFactory> dwriteFactory;
    check(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf())), "DWriteCreateFactory failed.");

    ComPtr<IWICImagingFactory> wicFactory;
    check(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory)), "CoCreateInstance WIC failed.");

    ComPtr<IDWriteTextFormat> format;
    check(dwriteFactory->CreateTextFormat(
              style.fontFamily.c_str(),
              nullptr,
              static_cast<DWRITE_FONT_WEIGHT>(std::clamp(style.fontWeight, 100, 999)),
              style.italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
              DWRITE_FONT_STRETCH_NORMAL,
              style.fontSize,
              L"",
              &format),
          "CreateTextFormat failed.");
    format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    ComPtr<IDWriteTextLayout> layout;
    check(dwriteFactory->CreateTextLayout(text.c_str(), static_cast<UINT32>(text.size()), format.Get(), 4096.0f, 4096.0f, &layout), "CreateTextLayout failed.");

    DWRITE_TEXT_METRICS metrics{};
    check(layout->GetMetrics(&metrics), "GetMetrics failed.");

    const float effect = style.padding + style.outlineWidth + style.glowWidth + 4.0f;
    const float skewExtra = std::abs(std::tan(style.skewDegrees * 3.1415926535f / 180.0f) * metrics.height);
    const int width = std::max(8, static_cast<int>(std::ceil(metrics.widthIncludingTrailingWhitespace + effect * 2.0f + skewExtra + std::abs(style.shadowOffsetX))));
    const int height = std::max(8, static_cast<int>(std::ceil(metrics.height + effect * 2.0f + std::abs(style.shadowOffsetY))));
    const float layoutX = effect + (style.skewDegrees < 0.0f ? skewExtra : 0.0f) - metrics.left;
    const float layoutY = effect - metrics.top;

    ComPtr<IWICBitmap> bitmap;
    check(wicFactory->CreateBitmap(width, height, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &bitmap), "Create WIC bitmap failed.");

    const D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

    ComPtr<ID2D1RenderTarget> target;
    check(d2dFactory->CreateWicBitmapRenderTarget(bitmap.Get(), properties, &target), "CreateWicBitmapRenderTarget failed.");

    ComPtr<ID2D1SolidColorBrush> outlineBrush;
    check(target->CreateSolidColorBrush(d2dColor(style.outlineColor), &outlineBrush), "Create outline brush failed.");

    ComPtr<ID2D1SolidColorBrush> shadowBrush;
    check(target->CreateSolidColorBrush(d2dColor(style.shadowColor), &shadowBrush), "Create shadow brush failed.");

    ComPtr<ID2D1SolidColorBrush> glowBrush;
    check(target->CreateSolidColorBrush(d2dColor(style.glowColor), &glowBrush), "Create glow brush failed.");

    D2D1_GRADIENT_STOP stops[2] = {
        {0.0f, d2dColor(style.fillTop)},
        {1.0f, d2dColor(style.fillBottom)},
    };
    ComPtr<ID2D1GradientStopCollection> stopCollection;
    check(target->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &stopCollection), "Create gradient stops failed.");

    ComPtr<ID2D1LinearGradientBrush> fillBrush;
    check(target->CreateLinearGradientBrush(
              D2D1::LinearGradientBrushProperties(D2D1::Point2F(0.0f, layoutY), D2D1::Point2F(0.0f, layoutY + metrics.height)),
              stopCollection.Get(),
              &fillBrush),
          "Create fill brush failed.");

    target->BeginDraw();
    target->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));

    const D2D1_MATRIX_3X2_F skew = D2D1::Matrix3x2F::Skew(style.skewDegrees, 0.0f, D2D1::Point2F(width * 0.5f, height * 0.5f));
    target->SetTransform(skew);

    drawLayout(d2dFactory.Get(), target.Get(), layout.Get(), shadowBrush.Get(), shadowBrush.Get(), style.outlineWidth + style.glowWidth, layoutX + style.shadowOffsetX, layoutY + style.shadowOffsetY);
    if (style.glowWidth > 0.0f) {
        drawLayout(d2dFactory.Get(), target.Get(), layout.Get(), nullptr, glowBrush.Get(), style.outlineWidth + style.glowWidth * 2.0f, layoutX, layoutY);
    }
    drawLayout(d2dFactory.Get(), target.Get(), layout.Get(), fillBrush.Get(), outlineBrush.Get(), style.outlineWidth, layoutX, layoutY);

    target->SetTransform(D2D1::Matrix3x2F::Identity());
    check(target->EndDraw(), "Direct2D EndDraw failed.");

    Image image;
    image.width = width;
    image.height = height;
    image.bgra.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4);

    WICRect rect{0, 0, width, height};
    check(bitmap->CopyPixels(&rect, static_cast<UINT>(width * 4), static_cast<UINT>(image.bgra.size()), image.bgra.data()), "CopyPixels failed.");

    for (std::size_t i = 0; i < image.bgra.size(); i += 4) {
        const int alpha = image.bgra[i + 3];
        if (alpha == 0) {
            image.bgra[i] = image.bgra[i + 1] = image.bgra[i + 2] = 0;
        } else if (alpha < 255) {
            image.bgra[i] = static_cast<std::uint8_t>(std::min(255, image.bgra[i] * 255 / alpha));
            image.bgra[i + 1] = static_cast<std::uint8_t>(std::min(255, image.bgra[i + 1] * 255 / alpha));
            image.bgra[i + 2] = static_cast<std::uint8_t>(std::min(255, image.bgra[i + 2] * 255 / alpha));
        }
    }

    return image;
}

} // namespace text2mdx
