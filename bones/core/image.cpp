#include "image.h"
#include "helper.h"
#include "rect.h"
#include "res_manager.h"
#include "css_utils.h"
#include "SkCanvas.h"
#include "SkColorMatrixFilter.h"

namespace bones
{

Image::Image() :style_(kDirect), delegate_(nullptr), color_filter_(nullptr)
{
    
}

Image::~Image()
{
    if (color_filter_)
        color_filter_->unref();
}

void Image::setDirect(const Point * bp)
{
    style_ = kDirect;
    if (bp)
        start_.set(bp->x(), bp->y());
    else
        start_.set(0, 0);
    inval();
}

void Image::setStretch(const Rect * dst)
{
    style_ = kStretch;
    if (dst)
        dst_ = *dst;
    else
        dst_.setEmpty();
    inval();
}

void Image::setNine(const Rect * dst, const Rect * center)
{
    style_ = kNine;
    if (dst)
        dst_ = *dst;
    else
        dst_.setEmpty();
    if (center)
        nine_ = *center;
    else
        nine_.setEmpty();
    inval();
}

void Image::set(const Pixmap & pm)
{
    pixmap_ = pm;
    inval();
}

void Image::set(const char * key)
{
    pixmap_ = Core::GetResManager()->getPixmap(key);
    inval();
}

void Image::setColorMatrix(const ColorMatrix & cm)
{
    if (color_filter_)
        color_filter_->unref();
    color_filter_ = SkColorMatrixFilter::Create(cm.mat);    
}

void Image::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

const char * Image::getClassName() const
{
    return kClassImage;
}

Image::DelegateBase * Image::delegate()
{
    return delegate_;
}

void Image::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    if (opacity == 0)
        return;

    Rect r;
    getLocalBounds(r);
    switch (style_)
    {
    case kDirect:
        drawDirect(canvas, r, opacity);
        break;
    case kStretch:
        drawStretch(canvas, r, opacity);
        break;
    case kNine:
        drawNine(canvas, r, opacity);
        break;
    }
}

void Image::drawDirect(SkCanvas & canvas, const Rect & bounds, float opacity)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity));
    if (color_filter_)
        paint.setColorFilter(color_filter_);
    canvas.drawBitmap(Helper::ToSkBitmap(pixmap_), start_.x(), start_.y(), &paint);
}

void Image::drawStretch(SkCanvas & canvas, const Rect & bounds, float opacity)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity));
    if (color_filter_)
        paint.setColorFilter(color_filter_);
    canvas.drawBitmapRect(Helper::ToSkBitmap(pixmap_), 0, 
        dst_.isEmpty() ? Helper::ToSkRect(bounds) : Helper::ToSkRect(dst_),
        &paint);
}

void Image::drawNine(SkCanvas & canvas, const Rect & bounds, float opacity)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity));
    if (color_filter_)
        paint.setColorFilter(color_filter_);
    Rect center(nine_);
    if (center.isEmpty())
    {
        Scalar sw = static_cast<Scalar>(pixmap_.getWidth()) / 3;
        Scalar sh = static_cast<Scalar>(pixmap_.getHeight()) / 3;
        center.setXYWH(sw, sh, sw, sh);
    }
    canvas.drawBitmapNine(Helper::ToSkBitmap(pixmap_),
        Helper::ToSkIRect(center),
        dst_.isEmpty() ? Helper::ToSkRect(bounds) : Helper::ToSkRect(dst_), 
        &paint);
}

BONES_CSS_TABLE_BEGIN(Image, View)
BONES_CSS_SET_FUNC("content", &Image::set)
BONES_CSS_TABLE_END()


void Image::set(const CSSParams & params)
{
    if (params.empty())
        return;
    set(CSSUtils::CSSStrToPixmap(params[0]));
}

}