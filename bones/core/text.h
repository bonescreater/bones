#ifndef BONES_CORE_TEXT_H_
#define BONES_CORE_TEXT_H_

#include "area.h"
#include "color.h"
#include "font.h"

namespace bones
{


class Text : public Area<Text>
{
public:
    enum Align
    {
        kLeft,//!<左对齐
        kCenter,//!<中间对齐
        kRight,//!<右对齐
    };

    enum Overflow
    {
        kNone,//超长不特殊处理
        kWordWrap,//超长自动换行
        kEllipsis,//超长以省略号代替
    };
    typedef std::wstring Line;
    typedef std::vector<Line> Lines;

public:
    class Delegate : public DelegateBase
    {

    };
public:
    Text();

    void set(const wchar_t * text);

    void setFont(const Font & font);

    void setColor(Color c);

    void setAlign(Align align);

    void setOverflow(Overflow of);

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onSizeChanged() override;
private:
    void adjustCache();

    void breakToLine();

    void appendEllipsis(size_t begin, size_t length);

    void wordWrap(size_t begin, size_t length);

    BONES_CSS_TABLE_DECLARE()

    void setColor(const CSSParams & params);

    void setContent(const CSSParams & params);

    void setOverflow(const CSSParams & params);

    void setFont(const CSSParams & params);

    void setAlign(const CSSParams & params);
private:
    Delegate * delegate_;
    bool cache_dirty_;
    std::wstring content_;
    Lines lines_;
    Color text_color_;
    Align text_align_;
    Overflow of_;
    Font font_;
};

}
#endif