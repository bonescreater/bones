#include "widget.h"
#include "size.h"
#include "point.h"
#include "rect.h"
#include "encoding.h"
#include "css_utils.h"

namespace bones
{

Widget::Widget() :hwnd_(NULL)
{

}

Widget::~Widget()
{
    ;
}

bool Widget::isValid() const
{
    return hwnd_ != NULL;
}


bool Widget::show(bool visible)
{
    if (visible)
        return !!::ShowWindow(hwnd_, SW_SHOW);
    else
        return !!::ShowWindow(hwnd_, SW_HIDE);
}

bool Widget::restore()
{
    return !!::ShowWindow(hwnd_, SW_RESTORE);
}

bool Widget::maximize()
{
    return !!::ShowWindow(hwnd_, SW_SHOWMAXIMIZED);
}

bool Widget::minimize()
{
    return !!::ShowWindow(hwnd_, SW_SHOWMINIMIZED);
}

bool Widget::close()
{
    return !!::CloseWindow(hwnd_);
}

bool Widget::setLeft(Scalar left)
{
    Rect wr;
    if (!getWindowRect(wr))
        return false;
    setLoc(Point::Make(left, wr.top()));
    Rect mr;
    getWindowRect(mr);
    return true;
}

bool Widget::setTop(Scalar top)
{
    Rect wr;
    if (!getWindowRect(wr))
        return false;
    return setLoc(Point::Make(wr.left(), top));
}

bool Widget::setLoc(const Point & pt)
{
    uint32_t flag = SWP_NOSIZE | SWP_NOZORDER |
        SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;

    return !!::SetWindowPos(hwnd_, 0, static_cast<int>(pt.x()),
        static_cast<int>(pt.y()), 0, 0, flag);
}

bool Widget::setWidth(Scalar width)
{
    Rect wr;
    if (!getWindowRect(wr))
        return false;
    return setSize(Size::Make(width, wr.height()));
}

bool Widget::setHeight(Scalar height)
{
    Rect wr;
    if (!getWindowRect(wr))
        return false;
    return setSize(Size::Make(wr.width(), height));
}

bool Widget::setSize(const Size & size)
{
    uint32_t flag = SWP_NOMOVE | SWP_NOZORDER |
        SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;

    return !!::SetWindowPos(hwnd_, 0, 0, 0,
        static_cast<int>(size.width()),
        static_cast<int>(size.height()), flag);
}

bool Widget::getWindowRect(Rect & rect)
{
    RECT wr = { 0 };
    auto result = !!::GetWindowRect(hwnd_, &wr);
    rect.setLTRB(static_cast<Scalar>(wr.left),
        static_cast<Scalar>(wr.top),
        static_cast<Scalar>(wr.right),
        static_cast<Scalar>(wr.bottom));
    return result;
}

bool Widget::getClientRect(Rect & rect)
{
    RECT cr = { 0 };
    auto result = !!::GetClientRect(hwnd_, &cr);
    rect.setLTRB(static_cast<Scalar>(cr.left),
        static_cast<Scalar>(cr.top),
        static_cast<Scalar>(cr.right),
        static_cast<Scalar>(cr.bottom));
    return result;
}

bool Widget::setWindowText(const wchar_t * text)
{
    return !!::SetWindowText(hwnd_, text);
}

void Widget::attach(HWND hwnd)
{
    hwnd_ = hwnd;
}

void Widget::detach()
{
    hwnd_ = NULL;
}

HWND Widget::hwnd() const
{
    return hwnd_;
}

const char * Widget::getClassName() const
{
    return kClassWidget;
}

BONES_CSS_BASE_TABLE_BEGIN(Widget)
BONES_CSS_SET_FUNC("left", &Widget::setLeft)
BONES_CSS_SET_FUNC("top", &Widget::setTop)
BONES_CSS_SET_FUNC("width", &Widget::setWidth)
BONES_CSS_SET_FUNC("height", &Widget::setHeight)
BONES_CSS_SET_FUNC("content", &Widget::setContent)
BONES_CSS_BASE_TABLE_END()

void Widget::setLeft(const CSSParams & params)
{
    if (params.empty())
        return;
    setLeft(CSSUtils::CSSStrToPX(params[0]));
}
void Widget::setTop(const CSSParams & params)
{
    if (params.empty())
        return;
    setTop(CSSUtils::CSSStrToPX(params[0]));
}
void Widget::setWidth(const CSSParams & params)
{
    if (params.empty())
        return;
    setWidth(CSSUtils::CSSStrToPX(params[0]));
}

void Widget::setHeight(const CSSParams & params)
{
    if (params.empty())
        return;
    setHeight(CSSUtils::CSSStrToPX(params[0]));
}

void Widget::setContent(const CSSParams & params)
{
    if (params.empty())
        return;
    setWindowText(Encoding::FromUTF8(params[0].begin, params[0].length).data());
}

}