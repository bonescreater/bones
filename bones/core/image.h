#ifndef BONES_CORE_IMAGE_H_
#define BONES_CORE_IMAGE_H_

#include "area.h"
#include "color.h"
#include "pixmap.h"
#include "rect.h"

class SkColorFilter;

namespace bones
{

class Image : public Area<Image>
{
public:
    class Delegate : public DelegateBase
    {

    };
private:
    enum Style
    {
        kDirect,//不进行处理
        kStretch,//填满
        kNine,//九宫格拉伸
    };
public:
    Image();

    ~Image();

    void setDirect(const Point * bp);

    void setStretch(const Rect * dst);

    void setNine(const Rect * dst, const Rect * center);

    void set(const Pixmap & pm);

    void set(const char * key);

    void setColorMatrix(const ColorMatrix & cm);

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;
protected:
    BONES_CSS_TABLE_DECLARE()

    void setStyle(const CSSParams & params);

    void set(const CSSParams & params);
private:
    void drawDirect(SkCanvas & canvas, const Rect & bounds, float opacity);

    void drawStretch(SkCanvas & canvas, const Rect & bounds, float opacity);

    void drawNine(SkCanvas & canvas, const Rect & bounds, float opacity);
private:
    Delegate * delegate_;
    Style style_;
    Pixmap pixmap_;
    Rect dst_;
    Rect nine_;
    Point start_;

    SkColorFilter * color_filter_;
};

}


#endif