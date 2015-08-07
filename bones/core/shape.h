#ifndef BONES_CORE_SHAPE_H_
#define BONES_CORE_SHAPE_H_


#include "view.h"
#include "color.h"
#include "rect.h"
#include "shader.h"

namespace bones
{

class Shape : public Skin
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

    void setMode(Mode mode);

    void setStyle(Style style);

    void setStrokeWidth(Scalar stroke_width);

    void setColor(Color color);

    void setShader(const Shader & shader);

    void set(Scalar rx, Scalar ry, const Rect * r = nullptr);

    void set(const Point & center, Scalar radius);

    void setBorder(Scalar width, Style style, Color color, Scalar rx, Scalar ry);

    const char * getClassName() const override;
protected:
    void onDraw(SkCanvas & canvas, const Rect & inval) override;

    void drawBackground(SkCanvas & canvas);

    void drawBorder(SkCanvas & canvas);
private:
    Category category_;
    Mode mode_;
    Style style_;
    RectParam rect_param_;
    CircleParam circle_param_;
    Scalar stroke_width_;

    ColourType colour_type_;
    Color color_;
    Shader shader_;

    Scalar border_width_;
    Style border_style_;
    Color border_color_;
    Scalar border_rx_;
    Scalar border_ry_;
};


}
#endif