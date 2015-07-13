#include "pixmap.h"
#include "SkMallocPixelRef.h"
#include <wincodec.h>

namespace bones
{

Pixmap::Pixmap()
:pixel_ref_(nullptr)
{
    ;
}

Pixmap::~Pixmap()
{
    free();
}

bool Pixmap::allocate(int width, int height, bool is_opaque)
{
    free();
    SkMallocPixelRef::PRFactory factory;
    pixel_ref_ = factory.create(
        SkImageInfo::Make(width, height, kPMColor_SkColorType,
                 is_opaque ? kOpaque_SkAlphaType : kPremul_SkAlphaType), nullptr);
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

int Pixmap::width() const
{
    return static_cast<int>(subset_.width());
}

int Pixmap::height() const
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

void Pixmap::erase(Color color)
{
    assert(0);
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


}