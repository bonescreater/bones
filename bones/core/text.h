#ifndef BONES_CORE_TEXT_H_
#define BONES_CORE_TEXT_H_

#include "view.h"
#include "color.h"

class SkPaint;
namespace bones
{


class Text : public Skin
{
public:
    enum Align
    {
        kLeft,//!<左对齐
        kCenter,//!<中间对齐
        kRight,//!<右对齐
    };
    enum FontStyle//字体样式
    {
        kNormal,//!<普通
        kBold,//!<粗体
        kItalic,//!<倾斜
        kBoldItalic,//!<粗体倾斜
    };

    enum OverFlow
    {
        kNone,//超长不特殊处理
        kWordWrap,//超长自动换行
        kEllipsis,//超长以省略号代替
    };
    typedef std::wstring Line;
    typedef std::vector<Line> Lines;
public:
    Text();

    const char * getClassName() const override;

    void set(const wchar_t * text);

    void setFontFamily(const char * family);

    void setFontStyle(FontStyle st);

    void setFontSize(Scalar s);

    void setColor(Color c);

    void setAlign(Align align);

    void setUnderline(bool ul);

    void setOverFlow(OverFlow of);
protected:
    void onDraw(SkCanvas & canvas, const Rect & inval) override;

    void onSizeChanged() override;
private:
    void adjustCache();

    void breakToLine();

    void appendEllipsis(size_t begin, size_t length);

    void wordWrap(size_t begin, size_t length);

    SkPaint ToSkPaint();
private:
    bool cache_dirty_;
    std::wstring content_;
    Lines lines_;
    std::string family_;
    FontStyle font_style_;
    Scalar text_size_;
    Color text_color_;
    Align text_align_;
    bool underline_;
    OverFlow of_;
};

}
#endif