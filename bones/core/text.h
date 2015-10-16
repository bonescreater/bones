#ifndef BONES_CORE_TEXT_H_
#define BONES_CORE_TEXT_H_

#include "area.h"
#include "color.h"
#include "font.h"

class SkPaint;
class SkShader;
struct SkPoint;

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
private:
    enum Mode
    {
        kAuto,//自动布局， 支持自动换行什么的
        kPos,//pos 在指定的位置显示
        kPath,//通过path显示文字
    };
public:
    class Delegate : public DelegateBase
    {

    };
public:
    Text();

    ~Text();

    void setFont(const Font & font);

    void setColor(Color c);

    void setColor(SkShader * shader);

    void set(const wchar_t * text, Scalar space, Align align, Overflow of);

    void set(const wchar_t * text, const Point * ps);

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onSizeChanged() override;
private:
    void drawPos(SkCanvas & canvas, SkPaint & paint);

    void drawAuto(SkCanvas & canvas, SkPaint & paint);

    void adjustCache();

    void breakToLine();

    void appendEllipsis(size_t begin, size_t length);

    void wordWrap(size_t begin, size_t length);

    BONES_CSS_TABLE_DECLARE()

    void setColor(const CSSParams & params);

    void setFont(const CSSParams & params);
private:
    Delegate * delegate_;
    bool cache_dirty_;
    std::wstring content_;
    Lines lines_;
    Overflow of_;
    Align align_;
    Color color_;
    SkShader * shader_;
    Font font_;
    Mode mode_;
    Scalar line_space_;
    std::vector<SkPoint> * pts_;
};

}
#endif