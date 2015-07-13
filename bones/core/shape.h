#ifndef BONES_CORE_SHAPE_H_
#define BONES_CORE_SHAPE_H_


#include "shirt.h"
#include "color.h"

namespace bones
{

class Shape : public Shirt
{
public:
    enum Style
    {
        kSolid,

    };

    enum Mode
    {
        kStroke,
        kFill,
    };
private:
    enum Category
    {
        kRect,//绘制矩形
        kCircle,
    };

    struct RectParam
    {
        Scalar rx;
        Scalar ry;
    };

    struct CircleParam
    {
        Point center;
        Scalar radius;
    };
public:
    Shape();

    void setMode(Mode mode);

    void setStyle(Style style);

    void setStrokeWidth(Scalar stroke_width);

    void setColor(Color color);

    void set(Scalar rx, Scalar ry);

    void set(const Point & center, Scalar radius);

    void setBorder(Scalar width, Style style, Color color, Scalar rx, Scalar ry);

    void setBorder(Scalar width, Style style, Color color);

    const char * getClassName() const override;
protected:
    void onDraw(SkCanvas & canvas) override;

    void drawBorder(SkCanvas & canvas);
private:
    Category category_;
    Mode mode_;
    Style style_;
    RectParam rect_param_;
    CircleParam circle_param_;
    Scalar stroke_width_;
    Color color_;

    Scalar border_width_;
    Style border_style_;
    Color border_color_;
    Scalar border_rx_;
    Scalar border_ry_;
};


}
#endif