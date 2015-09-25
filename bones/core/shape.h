#ifndef BONES_CORE_SHAPE_H_
#define BONES_CORE_SHAPE_H_


#include "area.h"
#include "color.h"
#include "rect.h"
#include "shader.h"

class SkPathEffect;
namespace bones
{

class Shape : public Area<Shape>
{
public:
    enum Effect
    {
        kSolid,
        kDash,
    };

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
    };

    enum ColourType
    {
        kColor,
        kShader,
    };

    struct RectParam
    {
        Scalar rx;
        Scalar ry;
        Rect rect;
    };

    struct CircleParam
    {
        Point center;
        Scalar radius;
    };
public:
    class Delegate : public DelegateBase
    {

    };
public:
    Shape();

    ~Shape();

    void setStyle(Style style);

    void setStrokeEffect(Effect effect, Scalar * interval, size_t count, Scalar offset);

    void setStrokeWidth(Scalar stroke_width);

    void setColor(Color color);

    void setShader(const Shader & shader);

    void set(Scalar rx, Scalar ry, const Rect * r = nullptr);

    void set(const Point & center, Scalar radius);

    void setBorder(Scalar width, Effect effect, Color color, Scalar rx, Scalar ry);

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void drawBackground(SkCanvas & canvas, float opacity);

    void drawBorder(SkCanvas & canvas, float opacity);

    BONES_CSS_TABLE_DECLARE()

    void setColor(const CSSParams & params);

    void setLinearGradient(const CSSParams & params);

    void setRadialGradient(const CSSParams & params);

    void setStyle(const CSSParams & params);

    void setStrokeEffect(const CSSParams & params);

    void setStrokeWidth(const CSSParams & params);

    void setRect(const CSSParams & params);

    void setCircle(const CSSParams & params);

    void setBorder(const CSSParams & params);
private:
    Delegate * delegate_;
    Category category_;
    Style style_;
    Scalar stroke_width_;
    RectParam rect_param_;
    CircleParam circle_param_;
    SkPathEffect * effect_;

    ColourType colour_type_;
    Color color_;
    Shader shader_;
    

    Scalar border_width_;
    Color border_color_;
    Scalar border_rx_;
    Scalar border_ry_;
    Effect border_effect_;
};


}
#endif