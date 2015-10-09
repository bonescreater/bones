
#include "root.h"
#include "rect.h"
#include "event.h"
#include "event_dispatcher.h"
#include "helper.h"
#include "SkCanvas.h"
#include "SkBitmap.h"
#include "device.h"
#include "SkDevice.h"
#include "logging.h"
#include <windowsx.h>
#include "css_utils.h"

namespace bones
{

Root::Root()
:mouse_(this), focus_(this), device_(nullptr), delegate_(nullptr),
widget_(NULL), color_(0)
{
    
}

Root::~Root()
{
    if (device_)
        device_->unref();
}

void Root::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

void Root::setColor(Color color)
{
    color_ = color;
    inval();
}

void Root::attachTo(Widget widget)
{
    widget_ = widget;
}

bool Root::handleMouse(NativeEvent & e)
{
    int flags = Helper::ToFlagsForMouseEvent();

    //暂时不管XBUTTON
    EventType et = kET_COUNT;
    MouseButton mb = kMB_NONE;

    Helper::ToEMForMouse(e.msg, et, mb);
    if (kET_COUNT == et)
    {
        BLG_VERBOSE << "Temporarily not supported: " << e.msg;
        return true;
    }
    auto & lparam = e.lparam;
    Point p(static_cast<Scalar>(GET_X_LPARAM(lparam)),
        static_cast<Scalar>(GET_Y_LPARAM(lparam)));
    MouseEvent me(et, mb, this, p, p, flags);
    me.setUserData(&e);
    mouse_.handleEvent(me);
    return true;
}

bool Root::handleKey(NativeEvent & e)
{
    //处理键盘事件
    EventType type = kET_COUNT;
    auto & msg = e.msg;
    if (WM_CHAR == msg || WM_SYSCHAR == msg)
        type = kET_CHAR;
    else if (WM_KEYDOWN == msg || WM_SYSKEYDOWN == msg)
        type = kET_KEY_DOWN;
    else if (WM_KEYUP == msg || WM_SYSKEYUP == msg)
        type = kET_KEY_UP;
    else
        return true;

    bool system = (msg == WM_SYSCHAR) || (msg == WM_SYSKEYDOWN) || (msg == WM_SYSKEYUP);
    auto flags = Helper::ToFlagsForKeyEvent(e.wparam, e.lparam);
    bool handle = false;

    View * focus = focus_.current();
    KeyEvent ke(type, focus, (KeyboardCode)e.wparam, *(KeyState *)(&e.lparam),
                system, flags);
    ke.setUserData(&e);
    if (kET_CHAR != type)
    {//非字符

        bool skip = false;        
        if (focus)
            skip = focus->skipDefaultKeyEventProcessing(ke);
        //焦点管理器会询问是否skip
        if (!skip)
        {//skip == false;
            //焦点管理器
            handle = focus_.handleKeyEvent(ke);
            if (!handle)//焦点管理器不处理 看快捷键管理器是否处理
                handle = accelerators_.process(Accelerator::make(ke));
        }
    }
    if (!handle && focus)
    {//都不处理
        KeyEvent ke(type, focus, (KeyboardCode)e.wparam, *(KeyState *)(&e.lparam),
            system, flags);
        EventDispatcher::Push(ke);
        handle = true;
    }
    return true;
}

bool Root::handleFocus(NativeEvent & e)
{
    auto & msg = e.msg;
    if (msg == WM_SETFOCUS)
    {
        has_focus_ = true;
    }
    else if (msg == WM_KILLFOCUS)
    {
        has_focus_ = false;
        //失去焦点 将内部焦点移除
        focus_.shift(nullptr);
    }
    return true;
}

bool Root::handleComposition(NativeEvent & e)
{
    View * focus = focus_.current();
    if (!focus)
        return true;
    auto & msg = e.msg;
    EventType type = kET_COUNT;
    if (WM_IME_STARTCOMPOSITION == msg)
        type = kET_COMPOSITION_START;
    else if (WM_IME_COMPOSITION == msg)
        type = kET_COMPOSITION_UPDATE;
    else if (WM_IME_ENDCOMPOSITION == msg)
        type = kET_COMPOSITION_END;
    else
        return true;

    CompositionEvent ce(type, focus);
    ce.setUserData(&e);
    EventDispatcher::Push(ce);

    return kClassWebView != focus->getClassName() || msg != WM_IME_COMPOSITION;
}

bool Root::handleWheel(NativeEvent & e)
{
    int flags = Helper::ToFlagsForMouseEvent();
    auto & msg = e.msg;
    if (WM_MOUSEWHEEL == msg || WM_MOUSEHWHEEL == msg)
    {
        auto type = kET_MOUSE_WHEEL;
        auto delta = GET_WHEEL_DELTA_WPARAM(e.wparam);
        Point p(static_cast<Scalar>(GET_X_LPARAM(e.lparam)), 
                static_cast<Scalar>(GET_Y_LPARAM(e.lparam)));

        WheelEvent we(type, this, WM_MOUSEHWHEEL == msg ? delta : 0,
            WM_MOUSEWHEEL == msg ? delta : 0, p, p, flags);

        //wheelEvent和mouseEvent分发逻辑一致
        we.setUserData(&e);
        mouse_.handleWheel(we);
    }
    return true;
}

Widget Root::getWidget()
{
    return widget_;
}

bool Root::isVisible() const
{
    return visible();
}

Root * Root::getRoot()
{
    return this;
}

const char * Root::getClassName() const
{
    return kClassRoot;
}

void Root::draw()
{
    adjustPixmap();
    //没有无效区
    if (!isDirty())
        return;

    Rect rect(dirty_);
    dirty_.setEmpty();
    //无交集
    Rect local_bounds;
    getLocalBounds(local_bounds);
    rect.intersect(local_bounds);
    if (rect.isEmpty())
        return;

    if (!back_buffer_.isValid())
        return;
    if (!device_)
        return;
    SkCanvas canvas(device_);
    View::draw(canvas, rect, 1.0f);
}

const Rect & Root::getDirtyRect() const
{
    return dirty_;
}

bool Root::isDirty() const
{
    return !dirty_.isEmpty();
}

const Surface & Root::getBackBuffer() const
{
    const_cast<Root *>(this)->adjustPixmap();
    return back_buffer_;
}

void Root::getBackBuffer(const void * & bits, size_t & pitch) const
{
    bits = bits_;
    pitch = pitch_;
}

void Root::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{  
    SkPaint paint;
    paint.setColor(color_);
    paint.setAlpha(ClampAlpha(opacity, ColorGetA(color_)));
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    canvas.drawPaint(paint);
}

void Root::onSizeChanged()
{
    delegate_ ? delegate_->onSizeChanged(this, getSize()) : 0;
}

void Root::onPositionChanged()
{
    delegate_ ? delegate_->onPositionChanged(this, getLoc()) : 0;
}

bool Root::notifyInval(const Rect & inval)
{
    dirty_.join(inval);
    delegate_ ? delegate_->invalidRect(this, inval) : 0;
    return true;
}

bool Root::notifyAddHierarchy(View * n)
{
    return true;
}

bool Root::notifyRemoveHierarchy(View * n)
{
    focus_.removed(n);
    mouse_.removed(n);
    return true;
}

bool Root::notifyVisibleChanged(View * n)
{
    if (n->visible())
        return true;
    focus_.shiftIfNecessary();
    mouse_.shiftIfNecessary();
    return true;
}

bool Root::notifySetFocus(View * n)
{
    if (!has_focus_)
        delegate_ ? delegate_->requestFocus(this) : 0;

    if (has_focus_)
        focus_.shift(n);
    return true;
}

bool Root::notifyChangeCursor(View * n, Cursor cursor)
{
    delegate_ ? delegate_->changeCursor(this, cursor) : 0;
    return true;
}

bool Root::notifyShowCaret(View * n, bool show)
{//应该是只有root 和focus才能显示光标 但是show 在windows 是要一一对应的
    //所以只能由view自己来负责显示以及隐藏
    delegate_ ? delegate_->showCaret(this, show) : true;
    return true;
}

bool Root::notifyChangeCaretPos(const Point & pt)
{
    delegate_ ? delegate_->changeCaretPos(this, pt) : 0;
    return true;
}

bool Root::notifyCreateCaret(Caret caret, const Size & size)
{
    delegate_ ? delegate_->createCaret(this ,caret, size) : 0;
    return true;
}

void Root::onVisibleChanged(View * n)
{
    assert(this == n);
    if (n->visible())
        return;
    focus_.shiftIfNecessary();
    mouse_.shiftIfNecessary();
}

void Root::adjustPixmap()
{
    int w = static_cast<int>(getWidth());
    int h = static_cast<int>(getHeight());
    if (w != back_buffer_.getWidth() || h != back_buffer_.getHeight())
    {
        if (device_)
        {
            device_->unref();
            device_ = nullptr;
        }
        back_buffer_.unlock();
        back_buffer_.free();
        if (w && h)
        {
            back_buffer_.allocate(w, h);
            device_ = Device::Create(back_buffer_);
            Pixmap::LockRec lr;
            back_buffer_.lock(lr);
            bits_ = lr.bits;
            pitch_ = lr.pitch;
        }            
    }
}

BONES_CSS_TABLE_BEGIN(Root, View)
BONES_CSS_SET_FUNC("color", &Root::setColor)
BONES_CSS_TABLE_END()

void Root::setColor(const CSSParams & params)
{
    if (params.empty())
        return;
    setColor(CSSUtils::CSSStrToColor(params[0]));
}

}