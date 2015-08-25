#ifndef BONES_CORE_SHAPE_H_
#define BONES_CORE_SHAPE_H_


#include "view.h"
#include "color.h"
#include "rect.h"
#include "shader.h"


class SkPathEffect;
namespace bones
{

class Shape : public View
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

    const char * getClassName() const override;
protected:
    void onDraw(SkCanvas & canvas, const Rect & inval) override;

    bool onHitTest(const Point & pt) override;

    void drawBackground(SkCanvas & canvas);

    void drawBorder(SkCanvas & canvas);

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
    SkPathEffect * border_effect_;
};


}
#endif