#include "image.h"
#include "helper.h"
#include "rect.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"

namespace bones
{

Image::Image() :style_(kImage)
{

}

Image::~Image()
{
    ;
}

void Image::set(Pixmap & pm)
{
    style_ = kImage;
    pixmap_ = pm;
    inval();
}

void Image::set(Pixmap & pm, Rect & nine_center)
{
    style_ = kImageNine;
    pixmap_ = pm;
    nine_center_ = nine_center;
    inval();
}

const char * Image::getClassName() const
{
    return kClassImage;
}

void Image::onDraw(SkCanvas & canvas, const Rect & inval)
{
    if (opacity_ == 0)
        return;

    Rect r;
    getLocalBounds(r);
    switch (style_)
    {
    case kImage:
        drawImage(canvas, r);
        break;
    case kImageNine:
        drawImageNine(canvas, r);
        break;
    }
}

void Image::drawImage(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));
    canvas.drawBitmap(Helper::ToSkBitmap(pixmap_), 0, 0, &paint);
}

void Image::drawImageNine(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));

    canvas.drawBitmapNine(Helper::ToSkBitmap(pixmap_),
        Helper::ToSkIRect(nine_center_),
        Helper::ToSkRect(bounds), &paint);
}

}