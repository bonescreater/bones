#include "shape.h"
#include "rect.h"
#include "helper.h"
#include "SkCanvas.h"
#include "css_utils.h"
#include "SkDashPathEffect.h"
#include <vector>

namespace bones
{

Shape::Shape()
:category_(kNone), style_(kFill), color_(0xff000000), stroke_width_(1),
border_width_(0), border_effect_(nullptr), border_color_(0xff000000), border_rx_(0), border_ry_(0),
colour_type_(kColor), effect_(nullptr)
{
    rect_param_.rx = 0;
    rect_param_.ry = 0;
    circle_param_.radius = 0;
    setFocusable(false);
}

Shape::~Shape()
{
    if (effect_)
        effect_->unref();
    effect_ = nullptr;
    if (border_effect_)
        border_effect_->unref();
    border_effect_ = nullptr;
}

void Shape::setStyle(Style style)
{
    style_ = style;
    inval();
}

void Shape::setStrokeEffect(Effect effect, Scalar * interval, size_t count, Scalar offset)
{
    if (effect_)
        effect_->unref();
    effect_ = nullptr;

    if (kDash == effect)
    {    
        if (interval && count)
            effect_ = SkDashPathEffect::Create(interval, count, offset);
        else
        {
            SkScalar default_interval[2] = { 2, 2 };
            effect_ = SkDashPathEffect::Create(default_interval, 2, 0);
        }
    }
    inval();
}

void Shape::setStrokeWidth(Scalar stroke_width)
{
    stroke_width_ = stroke_width;
    inval();
}

void Shape::setColor(Color color)
{
    colour_type_ = kColor;
    color_ = color;
    inval();
}

void Shape::setShader(const Shader & shader)
{
    colour_type_ = kShader;
    shader_ = shader;
    
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

void Shape::setBorder(Scalar width, Effect effect, Color color, Scalar rx, Scalar ry)
{
    border_width_ = width;
    border_color_ = color;
    border_rx_ = rx;
    border_ry_ = ry;
    //border effect 不支持自定义
    if (border_effect_)
        border_effect_->unref();
    border_effect_ = nullptr;
    if (kDash == effect)
    {
        SkScalar interval[2] = { 2, 2 };
        border_effect_ = SkDashPathEffect::Create(interval, 2, 0);
    }


    inval();
}

const char * Shape::getClassName() const
{
    return kClassShape;
}

void Shape::onDraw(SkCanvas & canvas, const Rect & inval)
{
    if (opacity_ == 0)
        return;
    drawBackground(canvas);
    drawBorder(canvas);
}

bool Shape::onHitTest(const Point & pt)
{
    return false;
}

void Shape::drawBackground(SkCanvas & canvas)
{
    if (kNone == category_)
        return;

    SkPaint paint;
    if (kShader == colour_type_)
    {
        paint.setShader(Helper::ToSkShader(shader_));
        paint.setAlpha(ClampAlpha(opacity_));
    }  
    else if (kColor == colour_type_)
    {
        paint.setColor(color_);
        paint.setAlpha(ClampAlpha(opacity_, ColorGetA(color_)));
    }
    else
        assert(0);

    paint.setStrokeWidth(stroke_width_);
    if (kFill == style_)
        paint.setStyle(SkPaint::kFill_Style);
    else if (kStroke == style_)
    {
        paint.setStyle(SkPaint::kStroke_Style);
        if (effect_)
            paint.setPathEffect(effect_);
    }
        

    if(kRect == category_)
    {
        SkRect r;
        if (rect_param_.rect.isEmpty())
        {
            Rect bounds;
            if (kStroke == style_)
            {
                auto offset = stroke_width_ / 2;
                bounds = Rect::MakeLTRB(offset, offset, getWidth() - offset, getHeight() - offset);
            }               
            else
                bounds = Rect::MakeLTRB(0, 0, getWidth(), getHeight());

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
    auto offset = border_width_ / 2;
    paint.setStrokeWidth(border_width_);
    paint.setPathEffect(border_effect_);
    Rect bounds;
    bounds.setLTRB(offset, offset, getWidth() - offset, getHeight() - offset);
    canvas.drawRoundRect(Helper::ToSkRect(bounds), border_rx_, border_ry_, paint);
}

BONES_CSS_TABLE_BEGIN(Shape, View)
BONES_CSS_SET_FUNC("border", &Shape::setBorder)
BONES_CSS_SET_FUNC("color", &Shape::setColor)
BONES_CSS_SET_FUNC("linear-gradient", &Shape::setLinearGradient)
BONES_CSS_SET_FUNC("radial-gradient", &Shape::setRadialGradient)
BONES_CSS_SET_FUNC("style", &Shape::setStyle)
BONES_CSS_SET_FUNC("stroke-effect", &Shape::setStrokeEffect)
BONES_CSS_SET_FUNC("stroke-width", &Shape::setStrokeWidth)
BONES_CSS_SET_FUNC("rect", &Shape::setRect)
BONES_CSS_SET_FUNC("circle", &Shape::setCircle)
BONES_CSS_TABLE_END()

static Shape::Effect CSSStrToEffect(const CSSString & str)
{
    if (str == "solid")
        return Shape::kSolid;
    else if (str == "dash")
        return Shape::kDash;

    return Shape::kSolid;
}

void Shape::setColor(const CSSParams & params)
{
    if (params.empty())
        return;
    setColor(CSSUtils::CSSStrToColor(params[0]));
}

void Shape::setLinearGradient(const CSSParams & params)
{
    if (params.empty())
        return;

    setShader(CSSUtils::CSSParamsToLinearGradientShader(params));
}

void Shape::setRadialGradient(const CSSParams & params)
{
    if (params.empty())
        return;
    setShader(CSSUtils::CSSParamsToRadialGradientShader(params));
}
//(stroke | fill, solid | dash)
void Shape::setStyle(const CSSParams & params)
{
    if (params.empty())
        return;
    auto & str = params[0];
    if (str == "fill")
        setStyle(Shape::kFill);
    if (str == "stroke")
        setStyle(Shape::kStroke);
}

void Shape::setStrokeEffect(const CSSParams & params)
{
    if (params.empty())
        return;
    Scalar * interval = nullptr;
    Scalar offset = 0;
    std::vector<Scalar> vecinterval;
    if (params.size() > 1)
    {
        for (size_t i = 1; i < params.size() - 1; ++i)
            vecinterval.push_back(CSSUtils::CSSStrToPX(params[i]));
        offset = CSSUtils::CSSStrToPX(params[params.size() - 1]);
        interval = &vecinterval[0];
    }
    setStrokeEffect(CSSStrToEffect(params[0]), interval, vecinterval.size(), offset);
}

void Shape::setStrokeWidth(const CSSParams & params)
{
    if (params.empty())
        return;
    setStrokeWidth(CSSUtils::CSSStrToPX(params[0]));
}
//
void Shape::setRect(const CSSParams & params)
{
    if (params.size() < 2)
        return;

    Rect * pr = nullptr;
    Rect r;
    if (params.size() > 5)
    {
        r.setLTRB(CSSUtils::CSSStrToPX(params[2]),
            CSSUtils::CSSStrToPX(params[3]),
            CSSUtils::CSSStrToPX(params[4]),
            CSSUtils::CSSStrToPX(params[5]));
        pr = &r;
    }
    set(CSSUtils::CSSStrToPX(params[0]), CSSUtils::CSSStrToPX(params[1]), pr);
}
//
void Shape::setCircle(const CSSParams & params)
{
    if (params.size() < 3)
        return;
    set(Point::Make(CSSUtils::CSSStrToPX(params[0]), CSSUtils::CSSStrToPX(params[1])),
        CSSUtils::CSSStrToPX(params[2]));
}

void Shape::setBorder(const CSSParams & params)
{
    if (params.size() < 3)
        return;
    Scalar rx = 0;
    if (params.size() > 3)
        rx = CSSUtils::CSSStrToPX(params[3]);
    Scalar ry = 0;
    if (params.size() > 4)
        rx = CSSUtils::CSSStrToPX(params[4]);

    setBorder(CSSUtils::CSSStrToPX(params[0]),
        CSSStrToEffect(params[1]),
        CSSUtils::CSSStrToColor(params[2]),
        rx, ry);
}

}