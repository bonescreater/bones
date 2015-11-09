﻿#include "input.h"
#include "helper.h"
#include "SkShader.h"
#include "SkCanvas.h"


namespace bones
{

Input::Input()
:delegate_(nullptr), max_scroll_(0), current_scroll_(0), 
caret_left_(0), color_(BONES_RGB_BLACK), shader_(nullptr),
select_begin_(0), select_end_(0), status_(kNormal),
password_(L'*'), pw_valid(false), display_caret_(true)
{

}

Input::~Input()
{
    if (shader_)
        shader_->unref();
}

void Input::setColor(Color c)
{
    //使用简单颜色
    color_ = c;
    if (shader_)
        shader_->unref();
    shader_ = nullptr;

    inval();
}

void Input::setColor(SkShader * shader)
{
    //使用渐变色  渐变色的alpha受到paint控制
    if (shader_)
        shader_->unref();
    shader_ = shader;
    if (shader_)
        shader_->ref();
    color_ = BONES_RGB_BLACK;

    inval();
}

void Input::setFont(const Font & font)
{
    font_ = font;

    adjustContentWidthsCache();
    if (kNormal == status_)
        moveContentCaretTo(select_begin_);
    inval();
}

void Input::set(const wchar_t * text)
{
    content_.clear();

    if (text)
        content_ = text;
    adjustContentWidthsCache();
    //更新最大可滚动范围
    setMaxScroll();
    //选中文本框的内容
    switchToSelect(0, content_.size());

    inval();
}

void Input::setPassword(bool pw, wchar_t password)
{
    pw_valid = pw;
    password_ = password;
    adjustContentWidthsCache();
    if (kNormal == status_)
        moveContentCaretTo(select_begin_);
    inval();
}

void Input::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

const char * Input::getClassName() const
{
    return kClassInput;
}

Input::DelegateBase * Input::delegate()
{
    return delegate_;
}

void Input::onMouseEnter(MouseEvent & e)
{
    Area::onMouseEnter(e);
    if (e.canceled())
        return;
}

void Input::onMouseLeave(MouseEvent & e)
{
    Area::onMouseLeave(e);
    //services_->OnTxInPlaceDeactivate();
}

void Input::onMouseMove(MouseEvent & e)
{
    Area::onMouseMove(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
}

void Input::onMouseDown(MouseEvent & e)
{
    Area::onMouseDown(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
}

void Input::onMouseUp(MouseEvent & e)
{
    Area::onMouseUp(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
}

void Input::onFocus(FocusEvent & e)
{
    Area::onFocus(e);
    if (e.canceled())
        return;
    //焦点到richedit时处于PlaceActivate
    if (Event::kTarget != e.phase())
        return;
}

void Input::onBlur(FocusEvent & e)
{
    Area::onBlur(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
}

void Input::onKeyDown(KeyEvent & e)
{
    Area::onKeyDown(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    switch (e.key())
    {
    case kVKEY_BACK:
        removeTextAtCaret();
        inval();
        break;
    case kVKEY_LEFT:
        moveContentCaret(false);
        inval();
        break;
    case kVKEY_RIGHT:
        moveContentCaret(true);
        inval();
        break;
    }
}

void Input::onKeyUp(KeyEvent & e)
{
    Area::onKeyUp(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
}

void Input::onChar(KeyEvent & e)
{
    Area::onChar(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;

    wchar_t value = e.ch();
    if (value != '\t')
    {//输入一个字符的话 
        bool is_printable = true;
        //过滤ASC中不可见字符
        if (value < 0x20 ||
            (value > 0x7e && value <= 0xff))
        {
            is_printable = false;
        }
        if (is_printable)
        {
            insertCharAtCaret(value);
            inval();
        }

    }

}

void Input::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    if (content_.empty())
        return;

    if (opacity == 0)
        return;
    canvas.translate(current_scroll_, 0);
    drawBackground(canvas, opacity);
    drawText(canvas, opacity);
    drawCaret(canvas, opacity);
}

void Input::drawBackground(SkCanvas & canvas, float opacity)
{
    //无选中块
    if (kSelect != status_)
        return;

    SkRect rect;
    SkPaint paint;
    ToSkPaint(paint);
    paint.setColor(BONES_RGB_BLUE);
    paint.setAlpha(ClampAlpha(opacity));

    //计算Rect
    auto begin = select_begin_ < select_end_ ? select_begin_ : select_end_;
    auto end = select_begin_ > select_end_ ? select_begin_ : select_end_;
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    Scalar text_height = fm.fBottom - fm.fTop;
    Scalar top = getHeight() / 2 - text_height / 2;
    rect.setLTRB(getOffsetOfContent(begin), top, getOffsetOfContent(end), top + text_height);
    //以蓝底绘制被选中的区域
    canvas.drawRect(rect, paint);
}

void Input::drawText(SkCanvas & canvas, float opacity)
{
    SkPaint paint;
    ToSkPaint(paint);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));

    //绘制单个字符
    Scalar left = 0;
    //垂直居中
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    Scalar text_height = fm.fBottom - fm.fTop;
    Scalar baseline = getHeight() / 2 - text_height / 2 - fm.fTop;

    for (size_t i = 0; i < content_.size(); ++i)
    {
        wchar_t ch = content_[i];
        if (pw_valid)
            ch = password_;

        if (kSelect == status_ && isInSelection(i))
        {
            paint.setColor(BONES_RGB_WHITE);
            paint.setShader(nullptr);
            paint.setAlpha(ClampAlpha(opacity));
        }
        else
        {
            paint.setColor(color_);
            paint.setShader(shader_);
            paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));
        }

        canvas.drawText(&ch, sizeof(ch), left, baseline, paint);
        left += content_widths_[i];

    }
}

void Input::drawCaret(SkCanvas & canvas, float opacity)
{
    if (kNormal != status_)
        return;
    if (display_caret_)
    {
        SkPaint paint;
        ToSkPaint(paint);
        paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));

        SkPaint::FontMetrics fm;
        paint.getFontMetrics(&fm);
        Scalar text_height = fm.fBottom - fm.fTop;
        Scalar top = getHeight() / 2 - text_height / 2;
        canvas.drawLine(caret_left_, top, caret_left_, top + text_height, paint);
    }
        
}

void Input::adjustContentWidthsCache()
{
    content_widths_.clear();
    if (content_.size())
    {
        auto size = content_.size();
        content_widths_.resize(size);
        SkPaint paint;
        ToSkPaint(paint);
        //auto m = paint.measureText(&content_[0], sizeof(content_[0]) * size);
        if (pw_valid)
        {//密码有效 则测量 密码字符串的宽
            std::wstring tmp;
            tmp.resize(size, password_);
            paint.getTextWidths(&tmp[0], sizeof(tmp[0]) * size,
                &content_widths_[0]);
        }
        else
            paint.getTextWidths(&content_[0], sizeof(content_[0]) * size,
                &content_widths_[0]);
        
    }
}

void Input::ToSkPaint(SkPaint & paint) const
{
    Helper::ToSkPaint(font_, paint);
    paint.setColor(color_);
    paint.setShader(shader_);
}

void Input::insertCharAtCaret(wchar_t ch)
{//在光标处插入一个字符
    if (kSelect == status_)
    {//先删除选中块的内容
        removeSelection();
        assert(select_begin_ == select_end_);
    }
    //现在开始插入字符
    content_.insert(select_begin_, 1, ch);
    adjustContentWidthsCache();
    setMaxScroll();
    //光标右移
    moveContentCaret(true);
}

void Input::removeTextAtCaret()
{
    if (kSelect == status_)
        removeSelection();
    else
        removeCharAtCaret();
}

void Input::removeSelection()
{
    if (kSelect == status_)
    {
        auto begin = select_begin_ < select_end_ ? select_begin_ : select_end_;
        auto end = select_begin_ > select_end_ ? select_begin_ : select_end_;
        //移动光标到选中块的右端
        moveContentCaretTo(end);
        //循环删除字符直到 光标移动到了begin处
        while (select_begin_ > begin)
            removeCharAtCaret();
    }
    else
        assert(0);
}

void Input::removeCharAtCaret()
{
    if (kNormal == status_)
    {
        if (select_begin_ > 0)
        {
            content_.erase(select_begin_ - 1, 1);
            adjustContentWidthsCache();
            setMaxScroll();
            moveContentCaretTo(select_begin_ - 1);
        }        
    }
    else
        assert(0);
    
}

void Input::moveContentCaret(bool right)
{
    int caret = 0;

    auto begin = select_begin_ < select_end_ ? select_begin_ : select_end_;
    auto end = select_begin_ > select_end_ ? select_begin_ : select_end_;

    if (right)
        caret = end + 1;
    else
        caret = begin - 1;
    moveContentCaretTo(caret);
}

//caret设置为起始位置
void Input::resetCaret()
{
    select_begin_ = select_end_ = 0;
    caret_left_ = 0;
}

Scalar Input::getOffsetOfContent(int index)
{
    if (index < 0)
        index = 0;
    if (index > static_cast<int>(content_.size()))
        index = content_.size();

    Scalar left = 0;
    if (index > 0)
    {
        for (int i = 0; i < index; ++i)
            left += content_widths_[i];
    }
    return left;
}

void Input::moveContentCaretTo(int index)
{
    switchToNormal(index);
}

void Input::switchToSelect(int begin, int end)
{
    status_ = kSelect;
    select_begin_ = begin;
    select_end_ = end;   
}

void Input::switchToNormal(int index)
{
    status_ = kNormal;
    if (index < 0)
        index = 0;
    if (index > static_cast<int>(content_.size()))
        index = content_.size();

    select_begin_ = select_end_ = index;
    caret_left_ = getOffsetOfContent(index);
    //更新光标后要保证光标在可显示区域内
    updateCaretPos();
}

bool Input::isInSelection(int index)
{
    auto begin = select_begin_ < select_end_ ? select_begin_ : select_end_;
    auto end = select_begin_ > select_end_ ? select_begin_ : select_end_;
    return index >= begin && index < end;
}

void Input::updateCaretPos()
{
    if (kNormal != status_)
        return;

    auto viewport_left = mapToScroll(0);
    auto viewport_right = mapToScroll(getWidth());
    if (caret_left_ < viewport_left)
    {//在显示区域内左边 向右滚动
        scroll(viewport_left - caret_left_);
    }
    else if (caret_left_ >= viewport_right)
    {//在显示区域右边 向左滚动
        scroll(viewport_right - caret_left_);
    }
}

Scalar Input::mapToScroll(Scalar x)
{
    return x - current_scroll_;
}

void Input::setMaxScroll()
{
    max_scroll_ = getWidth() - getOffsetOfContent(content_.size());
    if (max_scroll_ > 0)
        max_scroll_ = 0;
    if (current_scroll_ < max_scroll_)
        current_scroll_ = max_scroll_;
}

//- 向左 +向右
void Input::scroll(Scalar delta)
{
    current_scroll_ += delta;
    if (current_scroll_ > 0)
        current_scroll_ = 0;
    if (current_scroll_ < max_scroll_)
        current_scroll_ = max_scroll_;
}


}