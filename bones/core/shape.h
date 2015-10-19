#ifndef BONES_CORE_SHAPE_H_
#define BONES_CORE_SHAPE_H_


#include "area.h"
#include "color.h"
#include "rect.h"

class SkPathEffect;
class SkShader;
class SkPath;

namespace bones
{

class Shape : public Area<Shape>
{
public:
    enum Style
    {
        kStroke,
        kFill,
    };
private:
    enum Category
    {
        kNone,
        kRect,//绘制矩形
        kCircle,
        kLine,
        kPoint,
        kOval,
        kArc,
        kPath,
    };

    typedef union
    {
        struct Rect
        {
            Scalar rx;
            Scalar ry;
            Scalar left;
            Scalar top;
            Scalar right;
            Scalar bottom;
        }rect;

        struct Circle
        {
            Scalar cx;
            Scalar cy;
            Scalar radius;
        }circle;

        struct Point
        {
            Scalar x;
            Scalar y;
        } point;

        struct Line
        {
            Scalar xs;
            Scalar ys;
            Scalar xe;
            Scalar ye;
        } line;

        struct Oval
        {
            Scalar left;
            Scalar top;
            Scalar right;
            Scalar bottom;
        } oval;

        struct Arc
        {
            Scalar left;
            Scalar top;
            Scalar right;
            Scalar bottom;
            Scalar start;
            Scalar sweep;
            bool center;
        } arc;
    }Param;

public:
    class Delegate : public DelegateBase
    {

    };
public:
    Shape();

    ~Shape();

    void setStyle(Style style);

    void setStrokeEffect(SkPathEffect * effect);

    void setStrokeWidth(Scalar stroke_width);

    void setColor(Color color);

    void setColor(SkShader * shader);

    void set(Scalar rx, Scalar ry, const Rect * r = nullptr);

    void set(const Point & center, Scalar radius);

    void set(const Point & pt);

    void set(const Point & start, const Point & end);

    void set(const Rect * oval);

    void set(const Rect * oval, Scalar start, Scalar sweep, bool center);

    void set(const SkPath & path);

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void drawBackground(SkCanvas & canvas, float opacity);

    BONES_CSS_TABLE_DECLARE()

    void setColor(const CSSParams & params);

    void setLinearGradient(const CSSParams & params);

    void setRadialGradient(const CSSParams & params);

    void setStyle(const CSSParams & params);

    void setStrokeWidth(const CSSParams & params);

    void setRect(const CSSParams & params);

    void setCircle(const CSSParams & params);
private:
    Delegate * delegate_;
    Category category_;
    Style style_;
    Scalar stroke_width_;
    Param param_;
    SkPathEffect * effect_;

    Color color_;
    SkShader * shader_;
    SkPath * path_;
};


}
#endif