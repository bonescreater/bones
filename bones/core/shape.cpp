#include "shape.h"
#include "rect.h"
#include "helper.h"
#include "SkCanvas.h"
#include "SkGradientShader.h"

namespace bones
{

Shape::Shape()
:category_(kNone), mode_(kStroke), style_(kSolid), color_(0xff000000), stroke_width_(1),
border_width_(0), border_style_(kSolid), border_color_(0xff000000), border_rx_(0), border_ry_(0),
shader_(nullptr)
{
    rect_param_.rx = 0;
    rect_param_.ry = 0;
    circle_param_.radius = 0;
}

Shape::~Shape()
{
    if (shader_)
        shader_->unref();
}

void Shape::setMode(Mode mode)
{
    mode_ = mode;
    inval();
}

void Shape::setStyle(Style style)
{
    style_ = style;
    inval();
}

void Shape::setStrokeWidth(Scalar stroke_width)
{
    stroke_width_ = stroke_width;
    inval();
}

void Shape::setColor(Color color)
{
    color_ = color;
    if (shader_)
        shader_->unref();
    inval();
}

void Shape::setGradient(const Point & begin, const Point & end,
                              Color * color, float * pos, size_t count,
                              GradientTileMode tile)
{
    SkPoint pt[2] = { Helper::ToSkPoint(begin), Helper::ToSkPoint(end) };
    SkShader::TileMode mode = SkShader::kClamp_TileMode;
    if (kRepeat == tile)
        mode = SkShader::kRepeat_TileMode;
    else if (kMirror == tile)
        mode = SkShader::kMirror_TileMode;

    if (shader_)
        shader_->unref();
    shader_ = SkGradientShader::CreateLinear(pt, color, pos, count, mode);
    inval();
}

void Shape::setGradient(const Point & center, Scalar radius,
    Color * color, float * pos, size_t count, GradientTileMode tile)
{
    SkPoint pt = Helper::ToSkPoint(center);
    SkShader::TileMode mode = SkShader::kClamp_TileMode;
    if (kRepeat == tile)
        mode = SkShader::kRepeat_TileMode;
    else if (kMirror == tile)
        mode = SkShader::kMirror_TileMode;

    if (shader_)
        shader_->unref();
    shader_ = SkGradientShader::CreateRadial(pt, radius, color, pos, count, mode);
    inval();
}

void Shape::set(Scalar rx, Scalar ry, const Rect * r)
{   
    category_ = kRect;
    rect_param_.rx = rx;
    rect_param_.ry = ry;
    if (r)
        rect_param_.rect = *r;
    else
        rect_param_.rect.setEmpty();
    inval();
}

void Shape::set(const Point & center, Scalar radius)
{
    category_ = kCircle;
    circle_param_.center = center;
    circle_param_.radius = radius;

    inval();
}

void Shape::setBorder(Scalar width, Style style, Color color, Scalar rx, Scalar ry)
{
    border_width_ = width;
    border_style_ = style;
    border_color_ = color;
    border_rx_ = rx;
    border_ry_ = ry;

    inval();
}

const char * Shape::getClassName() const
{
    return kClassShape;
}

void Shape::onDraw(SkCanvas & canvas)
{
    if (opacity_ == 0)
        return;
    drawBackground(canvas);
    drawBorder(canvas);
}

void Shape::drawBackground(SkCanvas & canvas)
{
    if (kNone == category_)
        return;

    SkPaint paint;
    if (shader_)
    {
        paint.setShader(shader_);
        paint.setAlpha(ClampAlpha(opacity_));
    }  
    else
    {
        paint.setColor(color_);
        paint.setAlpha(ClampAlpha(opacity_, ColorGetA(color_)));
    }

    paint.setStrokeWidth(stroke_width_);
    if (kFill == mode_)
        paint.setStyle(SkPaint::kFill_Style);
    else if (kStroke == mode_)
        paint.setStyle(SkPaint::kStroke_Style);

    if(kRect == category_)
    {
        SkRect r;
        if (rect_param_.rect.isEmpty())
        {
            Rect bounds;
            getLocalBounds(bounds);
            r = Helper::ToSkRect(bounds);
        }
        else
            r = Helper::ToSkRect(rect_param_.rect);

        canvas.drawRoundRect(r, rect_param_.rx, rect_param_.ry, paint);
    }
    else if( kCircle == category_)
    {
        canvas.drawCircle(circle_param_.center.x(),
                          circle_param_.center.y(),
                          circle_param_.radius, paint);
    }
    
}

void Shape::drawBorder(SkCanvas & canvas)
{
    if (0 == border_width_)
        return;

    SkPaint paint;
    paint.setColor(border_color_);
    paint.setAlpha(ClampAlpha(opacity_, ColorGetA(border_color_)));
    paint.setStyle(SkPaint::kStroke_Style);
    //border_style_暂时用不到

    Rect bounds;
    bounds.setXYWH(0, 0, getWidth() - border_width_, getHeight() - border_width_);
    canvas.drawRoundRect(Helper::ToSkRect(bounds), border_rx_, border_ry_, paint);
}

}