#ifndef BONES_CORE_INPUT_H_
#define BONES_CORE_INPUT_H_

#include "area.h"
#include "font.h"

class SkShader;
class SkPaint;

namespace bones
{
//单行文本编辑 包含鼠标选中
class Input : public Area<Input>
{
public:
    class Delegate : public DelegateBase
    {

    };
public:
    Input();

    ~Input();

    void setFont(const Font & font);

    void setColor(Color c);

    void setColor(SkShader * shader);

    void set(const wchar_t * text);

    void setPassword(bool pw, wchar_t password);

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;
private:
    void adjustCache();

    void ToSkPaint(SkPaint & paint) const;
private:
    Delegate * delegate_;
    std::wstring content_;
    std::wstring composition_;//IME中的组合字符串
    Point caret_;//光标位置
    Color color_;//字体颜色
    SkShader * shader_;
    Font font_;//字体
    std::vector<Scalar> content_widths_;
    wchar_t password_;
    bool cache_dirty_;
    bool pw_valid;
};

}

#endif