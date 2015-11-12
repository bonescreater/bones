#include "input.h"
#include "helper.h"
#include "SkShader.h"
#include "SkCanvas.h"


namespace bones
{

Input::Input()
:delegate_(nullptr), max_scroll_(0), current_scroll_(0),
composition_start_(0), composition_length_(0),
caret_(0), color_(BONES_RGB_BLACK), shader_(nullptr),
select_begin_(0),
password_(L'*'), pw_valid(false),
left_down_(false)
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
    //更新最大可滚动范围
    setMaxScroll();
    //字体改变后 caret的位置可能变化
    updateCaretPos(caret_);
    //caret的大小也可能变化
    setCaretSize(getCaretSize());

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
    //caret的位置可能变化
    //updateCaretPos(0);
    switchToNormal(content_.size());
    inval();
}

void Input::setPassword(bool pw, wchar_t password)
{
    pw_valid = pw;
    password_ = password;
    adjustContentWidthsCache();
    //更新最大可滚动范围
    setMaxScroll();
    updateCaretPos(caret_);
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

void Input::onMouseLeave(MouseEvent & e)
{
    Area::onMouseLeave(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;

    left_down_ = false;
    inval();
}

void Input::onMouseMove(MouseEvent & e)
{
    Area::onMouseMove(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    if (left_down_)
    {//在input里按下左键移动
        switchToSelect(
            select_begin_,
            getIndexByOffset(mapToScroll(e.getLoc().x())));

        inval();
    }
}

void Input::onMouseDown(MouseEvent & e)
{
    Area::onMouseDown(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;

    if (e.isLeftMouse())
    {//左键按下
        if (!left_down_)
        {//左键未被按下
            left_down_ = true;
            //显示光标
            switchToNormal(getIndexByOffset(mapToScroll(e.getLoc().x())));
            inval();
        }
    }
}

void Input::onMouseUp(MouseEvent & e)
{
    Area::onMouseUp(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;

    if (e.isLeftMouse())
    {//左键弹起
        if (left_down_)
        {
            left_down_ = false;

            auto end = getIndexByOffset(mapToScroll(e.getLoc().x()));

            if (select_begin_ == end)
                switchToNormal(select_begin_);//显示光标
            else//有选中块则隐藏光标
                switchToSelect(select_begin_, end);

            inval();
        }

    }
}

void Input::onFocus(FocusEvent & e)
{
    Area::onFocus(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    createCaret();
    setCaretSize(getCaretSize());
    switchToSelect(0, content_.size());
}

void Input::onBlur(FocusEvent & e)
{
    Area::onBlur(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;

    destroyCaret();
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

void Input::onSizeChanged() 
{
    setMaxScroll();
    updateCaretPos(caret_);
}

void Input::onPositionChanged()
{//自己的位置改变后 如果caret正在闪烁也要修正到正确的位置
    updateCaretPos(caret_);
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
}

void Input::drawBackground(SkCanvas & canvas, float opacity)
{
    //无选中块
    if (caret_ == select_begin_)
        return;

    SkRect rect;
    SkPaint paint;
    ToSkPaint(paint);
    paint.setColor(BONES_RGB_BLUE);
    paint.setAlpha(ClampAlpha(opacity));

    //计算Rect
    auto begin = select_begin_ < caret_ ? select_begin_ : caret_;
    auto end = select_begin_ > caret_ ? select_begin_ : caret_;
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

        if (caret_ != select_begin_ && isInSelection(i))
        {//有选中块的情况而且要绘制的字符在选中块内
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

void Input::onDrawCaret(SkCanvas & canvas, const Rect & inval,
    const Size & size, float opacity)
{
    SkPaint paint;
    paint.setColor(color_);
    paint.setShader(nullptr);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));

    canvas.drawLine(0, 0, 0, size.height(), paint);
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
    if (caret_ != select_begin_)
    {//先删除选中块的内容
        removeSelection();
        assert(select_begin_ == caret_);
    }
    //现在开始插入字符
    content_.insert(caret_, 1, ch);
    adjustContentWidthsCache();
    setMaxScroll();
    //光标右移
    moveContentCaret(true);
    //强制显示光标
    switchToNormal(caret_);
}

void Input::removeTextAtCaret()
{
    if (caret_ != select_begin_)
    {
        removeSelection();
        switchToNormal(caret_);
    }       
    else
        removeCharAtCaret();
}

void Input::removeSelection()
{
    if (caret_ != select_begin_)
    {
        auto begin = select_begin_ < caret_ ? select_begin_ : caret_;
        auto end = select_begin_ > caret_ ? select_begin_ : caret_;
        //移动光标到选中块的右端
        updateCaretPos(end);

        //循环删除字符直到 光标移动到了begin处
        while (caret_ > begin)
            removeCharAtCaret();   
    }
    else
        assert(0);
}

void Input::removeCharAtCaret()
{
    if (caret_ > 0)
    {
        content_.erase(caret_ - 1, 1);
        adjustContentWidthsCache();
        setMaxScroll();
        updateCaretPos(caret_ - 1);
    }         
}

void Input::moveContentCaret(bool right)
{
    int caret = 0;

    auto begin = select_begin_ < caret_ ? select_begin_ : caret_;
    auto end = select_begin_ > caret_ ? select_begin_ : caret_;
    if (right)
        caret = checkIndex(end + 1);
    else
        caret = checkIndex(begin - 1);

    updateCaretPos(caret);
}

Scalar Input::getOffsetOfContent(int index)
{
    index = checkIndex(index);
    Scalar left = 0;
    if (index > 0)
    {
        for (int i = 0; i < index; ++i)
            left += content_widths_[i];
    }
    return left;
}

int Input::checkIndex(int index)
{
    if (index < 0)
        index = 0;
    if (index > static_cast<int>(content_.size()))
        index = content_.size();
    return index;
}

Size Input::getCaretSize()
{
    SkPaint paint;
    ToSkPaint(paint);
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    Scalar text_height = fm.fBottom - fm.fTop;

    return Size::Make(1, text_height);
}

Point Input::getCaretPoint()
{
    SkPaint paint;
    ToSkPaint(paint);
    SkPaint::FontMetrics fm;
    paint.getFontMetrics(&fm);
    Scalar text_height = fm.fBottom - fm.fTop;
    Scalar top = getHeight() / 2 - text_height / 2;
    auto offset = getOffsetOfContent(caret_);
    return Point::Make(mapToViewPort(offset), top);
}

void Input::switchToSelect(int begin, int end)
{//隐藏光标
    select_begin_ = checkIndex(begin);
    caret_  = checkIndex(end);
    updateCaretPos(caret_);
    showCaret(false);
}

void Input::switchToNormal(int index)
{//显示光标
    //更新光标后要保证光标在可显示区域内
    select_begin_ = caret_ = checkIndex(index);
    updateCaretPos(caret_);
    showCaret(true);
}

bool Input::isInSelection(int index)
{
    auto begin = select_begin_ < caret_ ? select_begin_ : caret_;
    auto end = select_begin_ > caret_ ? select_begin_ : caret_;
    return index >= begin && index < end;
}

//确保光标的位置在可显示区域 如果不在则滚动
void Input::updateCaretPos(int caret)
{
    auto ca = checkIndex(caret);
    auto caret_offset = getOffsetOfContent(ca);
    auto viewport_left = mapToScroll(0);
    auto viewport_right = mapToScroll(getWidth());
    if (caret_offset < viewport_left)
    {//在显示区域内左边 向右滚动
        scroll(viewport_left - caret_offset);
    }
    else if (caret_offset >= viewport_right)
    {//在显示区域右边 向左滚动
        scroll(viewport_right - caret_offset);
    }
    caret_ = ca;
    setCaretPos(getCaretPoint());
}

Scalar Input::mapToScroll(Scalar x)
{
    return x - current_scroll_;
}

Scalar Input::mapToViewPort(Scalar x)
{
    return x + current_scroll_;
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

int Input::getIndexByOffset(Scalar offset)
{
    if (offset <= 0)
        return 0;
    assert(content_widths_.size() == content_.size());

    Scalar left = 0;
    Scalar right = 0;
    for (size_t i = 0; i < content_widths_.size(); ++i)
    {
        right = left + content_widths_[i];
        if (offset >= left && offset < right)
        {
            Scalar middle = left + content_widths_[i] / 2;
            if (offset < middle)
                return i;
            else
                return i + 1;
        }
            
        left = right;
    }
    return content_.size();
}


}