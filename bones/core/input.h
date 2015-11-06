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
    void onMouseEnter(MouseEvent & e) override;

    void onMouseLeave(MouseEvent & e) override;

    void onMouseMove(MouseEvent & e) override;

    void onMouseDown(MouseEvent & e) override;

    void onMouseUp(MouseEvent & e) override;

    void onFocus(FocusEvent & e) override;

    void onBlur(FocusEvent & e) override;

    void onKeyDown(KeyEvent & e) override;

    void onKeyUp(KeyEvent & e) override;

    void onChar(KeyEvent & e) override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void drawText(SkCanvas & canvas, float opacity);

    void drawCaret(SkCanvas & canvas, float opacity);
private:
    void adjustContentWidthsCache();

    void ToSkPaint(SkPaint & paint) const;

    void insertCharAtCaret(wchar_t value);

    void removeTextAtCaret();

    void removeSelection();

    void removeCharAtCaret();
    
    void moveContentCaret(bool right);

    void resetCaret();

    Scalar getCaretXOfContent(int index);

    void moveContentCaretTo(int index);
private:
    Delegate * delegate_;
    std::wstring content_;
    std::wstring composition_;//IME中的组合字符串
    Scalar caret_left_;//光标位置
    Color color_;//字体颜色
    SkShader * shader_;
    Font font_;//字体
    std::vector<Scalar> content_widths_;
    //选中块处理
    int select_begin_;
    int select_end_;
    wchar_t password_;
    bool pw_valid;
    bool display_caret_;//是否显示光标
};

}

#endif