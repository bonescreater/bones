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

    void onSizeChanged() override;

    void onPositionChanged() override;

    void onCompositionStart(CompositionEvent & e) override;

    void onCompositionUpdate(CompositionEvent & e) override;

    void onCompositionEnd(CompositionEvent & e) override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onDrawCaret(SkCanvas & canvas,
                     const Size & size, float opacity) override;

    void drawBackground(SkCanvas & canvas, float opcatiy);

    void drawText(SkCanvas & canvas, float opacity); 
private:
    void adjustContentWidthsCache();

    void ToSkPaint(SkPaint & paint) const;

    void insertTextAtCaret(const wchar_t * str, size_t len);
    //光标前移删除字符串
    void removeTextAtCaret(size_t len);

    void removeTextAtCaret();

    void removeSelection();

    void moveContentCaret(bool right);

    Scalar getOffsetOfContent(int index);

    void moveContentCaretTo(int index);

    void switchToSelect(int begin, int end);

    void switchToNormal(int index);

    bool isInSelection(int index);

    void updateCaretPos(int caret);

    Scalar mapToScroll(Scalar x);

    Scalar mapToViewPort(Scalar x);

    void setMaxScroll();

    void scroll(Scalar delta);

    int getIndexByOffset(Scalar x);

    int checkIndex(int index);

    Size getCaretSize();

    Point getCaretPoint();
private:
    Delegate * delegate_;
    Scalar max_scroll_;//文本滚动支持
    Scalar current_scroll_;

    std::wstring content_;//包含composition
    int composition_start_;//composition 起始索引
    int composition_length_;//composition 长度

    Color color_;//字体颜色
    SkShader * shader_;
    Font font_;//字体
    //缓存宽度
    std::vector<Scalar> content_widths_;
    int caret_;//光标索引
    //选中块处理
    int select_begin_;
    
    wchar_t password_;
    bool pw_valid;
    bool left_down_;//左键是否在input中按下
};

}

#endif