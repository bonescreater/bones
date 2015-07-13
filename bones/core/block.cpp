#include "block.h"
#include "helper.h"
#include "rect.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"

namespace bones
{

Block::Block() :style_(kColor), color_(0xff0000), shader_(nullptr)
{

}

Block::~Block()
{
    if (shader_)
        shader_->unref();
}

void Block::set(Color color)
{
    style_ = kColor;
    color_ = color;
    inval();
}

void Block::set(const LinearGradient & linear)
{
    style_ = kShader;
    if (shader_)
        shader_->unref();
    shader_ = nullptr;

    linear_gradient_.begin[0] = linear.begin[0];
    linear_gradient_.begin[1] = linear.begin[1];
    linear_gradient_.end[0] = linear.end[0];
    linear_gradient_.end[1] = linear.end[1];
    linear_gradient_.count = linear.count;
    linear_gradient_.color_array.resize(linear.count);
    linear_gradient_.pos_array.resize(linear.count);
    for (size_t i = 0; i < linear.count; ++i)
    {
        linear_gradient_.color_array[i] = *(linear.color + i);
        linear_gradient_.pos_array[i] = *(linear.pos + i);
    }
    inval();
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
    case kColor:
        drawColor(canvas, r);
        break;
    case kShader:
        drawShader(canvas, r);
        break;
    case kImage:
        drawImage(canvas, r);
        break;
    case kImageNine:
        drawImageNine(canvas, r);
        break;
    }
}

void Block::onSizeChanged()
{
    if (shader_)
        shader_->unref();
    shader_ = nullptr;
}

void Block::drawColor(SkCanvas & canvas, const Rect & bounds)
{
    SkPaint paint;
    paint.setColor(color_);
    paint.setAlpha(ClampAlpha(opacity_ ,ColorGetA(color_)));

    canvas.drawRect(Helper::ToSkRect(bounds), paint);
}

void Block::drawShader(SkCanvas & canvas, const Rect & bounds)
{
    if (!shader_)
    {//创建1个shader
        SkPoint pt[2];
        pt[0].set(fromAnchor(linear_gradient_.begin[0]), fromAnchor(linear_gradient_.begin[1]));
        pt[1].set(fromAnchor(linear_gradient_.end[0]), fromAnchor(linear_gradient_.end[1]));

        shader_ = SkGradientShader::CreateLinear(pt,
            &linear_gradient_.color_array[0],
            &linear_gradient_.pos_array[0],
            linear_gradient_.count,
            SkShader::kClamp_TileMode);
    }
    SkPaint paint;
    paint.setShader(shader_);
    paint.setAlpha(ClampAlpha(opacity_));
    canvas.drawRect(Helper::ToSkRect(bounds), paint);
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

Scalar Block::fromAnchor(Anchor anchor)
{
    if (kLeft == anchor)
        return 0;
    else if (kTop == anchor)
        return 0;
    else if (kRight == anchor)
        return getWidth();
    else if (kBottom == anchor)
        return getHeight();
    else
        return 0;
}

}