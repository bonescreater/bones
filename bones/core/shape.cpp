#include "shape.h"
#include "rect.h"
#include "helper.h"
#include "SkCanvas.h"
#include "css_utils.h"
#include "SkDashPathEffect.h"
#include "SkShader.h"
#include "core_imp.h"

#include <vector>

namespace bones
{

Shape::Shape()
:category_(kNone), style_(kFill), color_(BONES_RGB_BLACK), stroke_width_(1),
effect_(nullptr), delegate_(nullptr), shader_(nullptr)
{
    //param 未初始化 category= kNone用不到param
}

Shape::~Shape()
{
    if (effect_)
        effect_->unref();
    if (shader_)
        shader_->unref();
}

void Shape::setStyle(Style style)
{
    style_ = style;
    inval();
}

void Shape::setStrokeEffect(SkPathEffect * effect)
{
    if (effect_)
        effect_->unref();
    effect_ = effect;
    if (effect_)
        effect_->ref();
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
    shader_ = nullptr;

    inval();
}

void Shape::setColor(SkShader * shader)
{
    if (shader_)
        shader_->unref();
    shader_ = shader;
    if (shader_)
        shader_->ref();
    color_ = BONES_RGB_BLACK;
    inval();
}

void Shape::set(Scalar rx, Scalar ry, const Rect * r)
{   
    category_ = kRect;
    param_.rect.rx = rx;
    param_.rect.ry = ry;
    if (r)
    {
        param_.rect.left = r->left();
        param_.rect.top = r->top();
        param_.rect.right = r->right();
        param_.rect.bottom = r->bottom();
    }     
    else
    {
        param_.rect.left = 0;
        param_.rect.top = 0;
        param_.rect.right = 0;
        param_.rect.bottom = 0;
    }
        
    inval();
}

void Shape::set(const Point & center, Scalar radius)
{
    category_ = kCircle;
    param_.circle.cx = center.x();
    param_.circle.cy = center.y();
    param_.circle.radius = radius;

    inval();
}

void Shape::set(const Point & pt)
{
    category_ = kPoint;
    param_.point.x = pt.x();
    param_.point.y = pt.y();
    inval();
}

void Shape::set(const Point & start, const Point & end)
{
    category_ = kLine;
    param_.line.xs = start.x();
    param_.line.ys = start.y();
    param_.line.xe = end.x();
    param_.line.ye = end.y();

    inval();
}

void Shape::set(const Rect * oval)
{
    category_ = kOval;
    if (oval)
    {
        param_.oval.left = oval->left();
        param_.oval.top = oval->top();
        param_.oval.right = oval->right();
        param_.oval.bottom = oval->bottom();
    }
    else
    {
        param_.oval.left = 0;
        param_.oval.top = 0;
        param_.oval.right = 0;
        param_.oval.bottom = 0;
    }
    inval();
}

void Shape::set(const Rect * oval, Scalar start, Scalar sweep, bool center)
{
    category_ = kArc;
    param_.arc.start = start;
    param_.arc.sweep = sweep;
    param_.arc.center = center;

    if (oval)
    {
        param_.arc.left = oval->left();
        param_.arc.top = oval->top();
        param_.arc.right = oval->right();
        param_.arc.bottom = oval->bottom();
    }
    else
    {
        param_.arc.left = 0;
        param_.arc.top = 0;
        param_.arc.right = 0;
        param_.arc.bottom = 0;
    }
    inval();
}

void Shape::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

Shape::DelegateBase * Shape::delegate()
{
    return delegate_;
}

const char * Shape::getClassName() const
{
    return kClassShape;
}

void Shape::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    if (opacity == 0)
        return;
    drawBackground(canvas, opacity);
}

void Shape::drawBackground(SkCanvas & canvas, float opacity)
{
    if (kNone == category_)
        return;

    SkPaint paint;
    paint.setAntiAlias(Core::AntiAliasEnable());
    paint.setColor(color_);
    paint.setShader(shader_);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));
    paint.setStrokeWidth(stroke_width_);
    if (effect_)
        paint.setPathEffect(effect_);

    if (kFill == style_)
        paint.setStyle(SkPaint::kFill_Style);
    else if (kStroke == style_)
        paint.setStyle(SkPaint::kStroke_Style);
        

    if(kRect == category_)
    {
        SkRect r;
        Rect re = Rect::MakeLTRB(param_.rect.left, 
                                 param_.rect.top, 
                                 param_.rect.right, 
                                 param_.rect.bottom);
        if (re.isEmpty())
        {
            if (kStroke == style_)
            {
                auto offset = stroke_width_ / 2;
                re = Rect::MakeLTRB(offset, offset, getWidth() - offset, getHeight() - offset);
            }               
            else
                re = Rect::MakeLTRB(0, 0, getWidth(), getHeight());

            r = Helper::ToSkRect(re);
        }
        else
        {
            if (kStroke == style_)
            {
                auto offset = stroke_width_ / 2;
                re = Rect::MakeLTRB(re.left() + offset, re.top() + offset, 
                                    re.right() - offset, re.bottom()- offset);
            }

            r = Helper::ToSkRect(re);

        }
            
        canvas.drawRoundRect(r, param_.rect.rx, param_.rect.ry, paint);
    }
    else if( kCircle == category_)
    {
        canvas.drawCircle(param_.circle.cx,
                          param_.circle.cy,
                          param_.circle.radius, paint);
    }
    else if (kLine == category_)
    {
        canvas.drawLine(param_.line.xs, param_.line.ys, 
                        param_.line.xe, param_.line.ye, 
                        paint);
    }
    else if (kPoint == category_)
    {
        canvas.drawPoint(param_.point.x, param_.point.y, paint);
    }
    else if (kOval == category_)
    {
        Rect re = Rect::MakeLTRB(param_.oval.left,
            param_.oval.top,
            param_.oval.right,
            param_.oval.bottom);
        if (re.isEmpty())
            re.setLTRB(0, 0, getWidth(), getHeight());

        canvas.drawOval(Helper::ToSkRect(re), paint);
    }
    else if (kArc == category_)
    {
        Rect re = Rect::MakeLTRB(param_.arc.left,
            param_.arc.top,
            param_.arc.right,
            param_.arc.bottom);
        if (re.isEmpty())
            re.setLTRB(0, 0, getWidth(), getHeight());

        canvas.drawArc(Helper::ToSkRect(re),
            param_.arc.start, param_.arc.sweep, param_.arc.center, paint);
    }
}

BONES_CSS_TABLE_BEGIN(Shape, View)
BONES_CSS_SET_FUNC("color", &Shape::setColor)
BONES_CSS_SET_FUNC("style", &Shape::setStyle)
BONES_CSS_SET_FUNC("stroke-width", &Shape::setStrokeWidth)
BONES_CSS_SET_FUNC("rect", &Shape::setRect)
BONES_CSS_SET_FUNC("circle", &Shape::setCircle)
BONES_CSS_TABLE_END()

void Shape::setColor(const CSSParams & params)
{
    if (params.empty())
        return;
    setColor(CSSUtils::CSSStrToColor(params[0]));
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
        r = CSSUtils::CSSStrToPX(params[2], params[3], params[4], params[5]);
        pr = &r;
    }
    set(CSSUtils::CSSStrToPX(params[0]), CSSUtils::CSSStrToPX(params[1]), pr);
}
//
void Shape::setCircle(const CSSParams & params)
{
    if (params.size() < 3)
        return;
    set(CSSUtils::CSSStrToPX(params[0], params[1]),
        CSSUtils::CSSStrToPX(params[2]));
}

}