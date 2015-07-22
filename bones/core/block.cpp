#include "block.h"
#include "helper.h"
#include "rect.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"

namespace bones
{

Block::Block() :style_(kImage)
{

}

Block::~Block()
{
    ;
}

void Block::set(Pixmap & pm)
{
    style_ = kImage;
    pixmap_ = pm;
    inval();
}

void Block::set(Pixmap & pm, Rect & nine_center)
{
    style_ = kImageNine;
    pixmap_ = pm;
    nine_center_ = nine_center;
    inval();
}

const char * Block::getClassName() const
{
    return kClassBlock;
}

void Block::onDraw(SkCanvas & canvas)
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

void Block::drawImage(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));
    canvas.drawBitmap(Helper::ToSkBitmap(pixmap_), 0, 0, &paint);
}

void Block::drawImageNine(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity_));

    canvas.drawBitmapNine(Helper::ToSkBitmap(pixmap_),
        Helper::ToSkIRect(nine_center_),
        Helper::ToSkRect(bounds), &paint);
}

}