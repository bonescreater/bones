#include "image.h"
#include "helper.h"
#include "rect.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"
#include "css_utils.h"

namespace bones
{

Image::Image() :style_(kNone)
{
    setFocusable(false);
}

Image::~Image()
{
    ;
}

void Image::setStyle(Style & style, const Rect * r)
{
    style_ = style;
    if (kNine == style_)
    {
        if (r)
            nine_center_ = *r;
        else
            nine_center_.setEmpty();
    }
    inval();
}

void Image::set(Pixmap & pm)
{
    pixmap_ = pm;
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
    case kNone:
        drawNone(canvas, r);
        break;
    case kFill:
        drawFill(canvas, r);
        break;
    case kNine:
        drawNine(canvas, r);
        break;
    }
}

bool Image::onHitTest(const Point & pt)
{
    return false;
}

void Image::drawNone(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));
    canvas.drawBitmap(Helper::ToSkBitmap(pixmap_), 0, 0, &paint);
}

void Image::drawFill(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));
    canvas.drawBitmapRect(Helper::ToSkBitmap(pixmap_), 0, Helper::ToSkRect(bounds), &paint);
}

void Image::drawNine(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));
    Rect center(nine_center_);
    if (center.isEmpty())
    {
        Scalar sw = static_cast<Scalar>(pixmap_.getWidth()) / 3;
        Scalar sh = static_cast<Scalar>(pixmap_.getHeight()) / 3;
        center.setXYWH(sw, sh, sw, sh);
    }
    canvas.drawBitmapNine(Helper::ToSkBitmap(pixmap_),
        Helper::ToSkIRect(center),
        Helper::ToSkRect(bounds), &paint);
}

BONES_CSS_TABLE_BEGIN(Image, View)
BONES_CSS_SET_FUNC("style", &Image::setStyle)
BONES_CSS_SET_FUNC("content", &Image::set)
BONES_CSS_TABLE_END()

void Image::setStyle(const CSSParams & params)
{
    if (params.empty())
        return;
    auto & str_style = params[0];
    Style style = kNone;
    if (str_style == "fill")
        style = kFill;
    else if (str_style == "nine")
        style = kNine;
    Rect * r = nullptr;
    Rect center;
    if (params.size() > 5)
    {
        center = CSSUtils::CSSStrToPX(params[1], params[2], params[3], params[4]);
        r = &center;
    }
    setStyle(style, r);
}

void Image::set(const CSSParams & params)
{
    if (params.empty())
        return;
    set(CSSUtils::CSSStrToPixmap(params[0]));
}

}