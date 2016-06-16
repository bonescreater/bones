#ifndef BONES_CORE_TEXT_H_
#define BONES_CORE_TEXT_H_

#include "area.h"
#include "color.h"
#include "font.h"

class SkPaint;
class SkShader;
//内部使用vector<Scalar>来模拟SkPoint 保留这个声明 万一SkPoint 变为class 编译会有警告
struct SkPoint;
class SkPath;

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
    typedef std::wstring Line;
    typedef std::vector<Line> Lines;
private:
    enum Mode
    {
        kAuto,//自动布局， 支持自动换行什么的
        kFloat,
        kPos,//pos 在指定的位置显示
        kPath,//通过path显示文字
    };
public:
    class Delegate : public DelegateBase
    {

    };
public:
    Text(ThreadContext & context);

    ~Text();

    void setFont(const Font & font);

    void setColor(Color c);

    void setColor(SkShader * shader);

    void setLineSpace(Scalar space);

    void set(const wchar_t * text);

    void setAuto(Align align, bool ellipsis);

    void setFloat();

    void setPos(const Point * ps, size_t count);

    void setPath(const SkPath & path);

    void setDelegate(Delegate * delegate);

    Rect getFloatBounds(Scalar max_width) const;

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onSizeChanged() override;
private:
    void drawPos(SkCanvas & canvas, SkPaint & paint);

    void drawFloat(SkCanvas & canvas, SkPaint & paint);

    void drawAuto(SkCanvas & canvas, SkPaint & paint);

    void drawPath(SkCanvas & canvas, SkPaint & paint);

    void adjustCache();

    void adjustEllipsisCache();

    void adjustWordWrapCache();

    void appendEllipsis(size_t begin, size_t length);

    Scalar wordWrap(size_t begin, size_t length, 
        Scalar max_width, Lines & ls) const;

    void ToSkPaint(SkPaint & paint) const;

    BONES_CSS_TABLE_DECLARE()

    void setColor(const CSSParams & params);

    void setFont(const CSSParams & params);
private:
    Delegate * delegate_;  
    std::wstring content_;
    Lines lines_;
    Align align_;
    Color color_;
    SkShader * shader_;
    Font font_;
    Mode mode_;
    Scalar line_space_;
    SkPath * path_;
    std::vector<SkPoint> * pts_;
    bool cache_dirty_;
    bool ellipsis_;//是否以省略号显示
};

}
#endif