#ifndef BONES_CORE_IMAGE_H_
#define BONES_CORE_IMAGE_H_

#include "view.h"
#include "color.h"
#include "pixmap.h"
#include "rect.h"

class SkShader;

namespace bones
{

class Image : public View
{
private:
    enum Style
    {
        kNone,//不进行处理
        kFill,//填满
        kNine,//九宫格拉伸
    };
public:
    Image();

    ~Image();

    void setStyle(Style & style, const Rect * r);

    void set(Pixmap & pm);

    const char * getClassName() const override;
protected:
    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    bool onHitTest(const Point & pt) override;
protected:
    BONES_CSS_TABLE_DECLARE()

    void setStyle(const CSSParams & params);

    void set(const CSSParams & params);
private:
    void drawNone(SkCanvas & canvas, const Rect & bounds, float opacity);

    void drawFill(SkCanvas & canvas, const Rect & bounds, float opacity);

    void drawNine(SkCanvas & canvas, const Rect & bounds, float opacity);
private:
    Style style_;
    Pixmap pixmap_;
    Rect nine_center_;
};

}


#endif