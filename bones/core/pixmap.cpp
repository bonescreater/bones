#include "pixmap.h"
#include "SkMallocPixelRef.h"
#include "SkUtils.h"
#include <wincodec.h>

namespace bones
{

Pixmap::Pixmap()
:pixel_ref_(nullptr)
{
    ;
}

Pixmap::Pixmap(const Pixmap & pm)
: Pixmap()
{
    *this = pm;
}

Pixmap::~Pixmap()
{
    free();
}

bool Pixmap::allocate(int width, int height, bool is_opaque)
{
    free();
    //防止分配0*0的位图
    if (0 == width)
        width = 4;
    if (0 == height)
        height = 4;

    pixel_ref_ = allocatePixelRef(width, height, is_opaque);
    subset_.setXYWH(0, 0, static_cast<Scalar>(width), static_cast<Scalar>(height));
    return !!pixel_ref_;
}

void Pixmap::free()
{
    if (pixel_ref_)
        pixel_ref_->unref();
    pixel_ref_ = nullptr;
    subset_.setEmpty();
}

bool Pixmap::lock(LockRec & rec)
{
    if (!pixel_ref_)
        return false;
    SkPixelRef::LockRec lr;
    if (!pixel_ref_->lockPixels(&lr))
        return false;
    rec.bits = lr.fPixels;
    rec.pitch = lr.fRowBytes;
    rec.subset = subset_;
    return true;
}

void Pixmap::unlock()
{
    if (pixel_ref_)
        pixel_ref_->unlockPixels();
}

bool Pixmap::isValid() const
{
    return !!pixel_ref_;
}

bool Pixmap::isEmpty() const
{
    return subset_.isEmpty();
}

int Pixmap::getWidth() const
{
    return static_cast<int>(subset_.width());
}

int Pixmap::getHeight() const
{
    return static_cast<int>(subset_.height());
}

Pixmap Pixmap::extractSubset(const Rect& subset)
{
    Pixmap sub;

    Rect real_subset = subset_;
    real_subset.intersect(subset);
    if (!real_subset.isEmpty())
    {
        sub.subset_ = real_subset;
        sub.pixel_ref_ = pixel_ref_;
        if (sub.pixel_ref_)
            sub.pixel_ref_->ref();
    }

    return sub;
}

SkPixelRef * Pixmap::allocatePixelRef(int width, int height, bool is_opaque)
{
    SkMallocPixelRef::PRFactory factory;
    return factory.create(
        SkImageInfo::Make(width, height, kPMColor_SkColorType,
        is_opaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType), nullptr);
}

void Pixmap::erase(Color color)
{
    if (!pixel_ref_)
        return;

    LockRec rec;
    if (!lock(rec))
        return;

    uint32_t precolor = SkPreMultiplyColor(color);
    auto width = subset_.width();
    auto height = subset_.height();
    auto pitch = rec.pitch;
    size_t offset = (size_t)(subset_.left() * 4 + subset_.top() * pitch);
    uint32_t * bits = static_cast<uint32_t*>(rec.bits);

    bits = (uint32_t *)((char *)bits + offset);
    while (height)
    {
        sk_memset32(bits, precolor, (int)width);
        bits = (uint32_t *)((char *)bits + pitch);
        height -= 1;
    }
    unlock();
}

Pixmap & Pixmap::operator=(const Pixmap & pm)
{
    subset_ = pm.subset_;
    if (pixel_ref_)
        pixel_ref_->unref();
    pixel_ref_ = pm.pixel_ref_;
    if (pixel_ref_)
        pixel_ref_->ref();
    return *this;
}

bool Pixmap::decode(const void * buffer, size_t len)
{
    auto co_hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(co_hr) && !(RPC_E_CHANGED_MODE == co_hr))
        return false;

    IWICImagingFactory * wic_factory = nullptr;
    IWICStream * stream = nullptr;
    IWICBitmapDecoder * decoder = nullptr;
    IWICBitmapFrameDecode * source = nullptr;
    IWICFormatConverter * converter = nullptr;

    auto hr = ::CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&wic_factory)
        //IID_IWICImagingFactory,
        //reinterpret_cast<void **>(&wic_factory)
        );

    if (SUCCEEDED(hr))
    {
        // Create a WIC stream to map onto the memory.
        hr = wic_factory->CreateStream(&stream);
    }
    if (SUCCEEDED(hr))
    {
        // Initialize the stream with the memory pointer and size.
        hr = stream->InitializeFromMemory(
            reinterpret_cast<BYTE *>(const_cast<void*>(buffer)),
            len
            );
    }
    if (SUCCEEDED(hr))
    {
        // Create a decoder for the stream.
        hr = wic_factory->CreateDecoderFromStream(
            stream,
            NULL,
            WICDecodeMetadataCacheOnDemand,
            &decoder
            );
    }

    if (SUCCEEDED(hr))
    {
        // Create the initial frame.
        hr = decoder->GetFrame(0, &source);
    }

    if (SUCCEEDED(hr))
    {
        // Convert the image format to 32bppPBGRA
        // (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
        hr = wic_factory->CreateFormatConverter(&converter);
    }
    if (SUCCEEDED(hr))
    {
        hr = converter->Initialize(
            source,
            GUID_WICPixelFormat32bppPBGRA,
            WICBitmapDitherTypeNone,
            NULL,
            0.f,
            WICBitmapPaletteTypeMedianCut
            );
    }

    if (SUCCEEDED(hr))
    {
        uint32_t width, height;
        converter->GetSize(&width, &height);
        if (allocate(width, height))
        {
            converter->CopyPixels(NULL, pixel_ref_->rowBytes(),
                pixel_ref_->rowBytes() * height, (BYTE *)pixel_ref_->pixels());
        }
    }

    if (wic_factory)
        wic_factory->Release();
    if (stream)
        stream->Release();
    if (decoder)
        decoder->Release();
    if (source)
        source->Release();
    if (converter)
        converter->Release();

    if (SUCCEEDED(co_hr))
        CoUninitialize();

    return SUCCEEDED(hr);
}


Surface::Surface()
:context_(nullptr)
{
    ;
}

Surface & Surface::operator=(const Surface & right)
{
    Pixmap * pl = this;
    Pixmap * pr = (Pixmap *)&right;
    *pl = *pr;
    this->context_ = right.context_;
    return *this;
}

SkPixelRef * Surface::allocatePixelRef(int width, int height, bool is_opaque)
{
    BITMAPINFOHEADER hdr = { 0 };
    hdr.biSize = sizeof(BITMAPINFOHEADER);
    hdr.biWidth = width;
    hdr.biHeight = -height;  // minus means top-down bitmap
    hdr.biPlanes = 1;
    hdr.biBitCount = 32;
    hdr.biCompression = BI_RGB;  // no compression
    hdr.biSizeImage = 0;
    hdr.biXPelsPerMeter = 1;
    hdr.biYPelsPerMeter = 1;
    hdr.biClrUsed = 0;
    hdr.biClrImportant = 0;
    void * data = nullptr;
    auto hbm = CreateDIBSection(NULL, reinterpret_cast<BITMAPINFO*>(&hdr),
        0, &data, NULL, 0);
    context_ = new Context;
    context_->dc = ::CreateCompatibleDC(NULL);
    context_->obj = ::SelectObject(context_->dc, hbm);
    ::SetGraphicsMode(context_->dc, GM_ADVANCED);

    auto info = SkImageInfo::Make(width, height, kPMColor_SkColorType,
        is_opaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType);
    return SkMallocPixelRef::NewWithProc(info, info.minRowBytes(), nullptr,
        data, &Surface::PixelRefFree, context_);
}

void Surface::PixelRefFree(void * addr, void * context)
{
    if (context)
    {
        Context * c = (Context *)context; 
        ::DeleteObject(::SelectObject(c->dc, c->obj));
        ::DeleteDC(c->dc);
        delete c;
    }
        
}

}