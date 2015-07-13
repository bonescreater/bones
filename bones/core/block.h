#ifndef BONES_CORE_BLOCK_H_
#define BONES_CORE_BLOCK_H_

#include "shirt.h"
#include "color.h"
#include "pixmap.h"
#include "rect.h"

class SkShader;

namespace bones
{

class Block : public Shirt
{
private:
    enum Style
    {
        kColor,
        kShader,
        kImage,
        kImageNine,
    };
public:
    enum Anchor
    {
        kLeft,
        kTop,
        kRight,
        kBottom,
    };
    struct LinearGradient
    {
        Anchor begin[2];
        Anchor end[2];
        Color * color;
        Scalar * pos;
        size_t count;
    };
private:
    struct LinearGradientPri
    {
        Anchor begin[2];
        Anchor end[2];
        std::vector<Color>color_array;
        std::vector<Scalar>pos_array;
        size_t count;
    };
public:
    Block();

    ~Block();

    void set(Color color);//单色

    void set(const LinearGradient & linear);

    void set(Pixmap & pm);//直接绘制 不拉伸

    void set(Pixmap & pm, Rect & nine_center);//九宫格拉伸

    const char * getClassName() const override;
protected:
    void onDraw(SkCanvas & canvas) override;

    void onSizeChanged() override;
private:
    void drawColor(SkCanvas & canvas, const Rect & bounds);

    void drawShader(SkCanvas & canvas, const Rect & bounds);

    void drawImage(SkCanvas & canvas, const Rect & bounds);

    void drawImageNine(SkCanvas & canvas, const Rect & bounds);

    Scalar fromAnchor(Anchor anchor);
private:
    Style style_;
    Color color_;
    SkShader * shader_;
    LinearGradientPri linear_gradient_;
    Pixmap pixmap_;
    Rect nine_center_;
};

}


#endif