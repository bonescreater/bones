
#include "root_view.h"
#include "rect.h"
#include "event.h"
#include "event_dispatcher.h"
#include "helper.h"
#include "SkCanvas.h"
#include "SkBitmap.h"
#include "device.h"
#include "SkDevice.h"

namespace bones
{

RootView::RootView()
: delegate_(nullptr), mouse_(this), focus_(this),
device_(nullptr)
{
    ;
}
RootView::~RootView()
{
    if (device_)
        device_->unref();
}

Widget * RootView::getWidget() const
{
    return delegate_ ? delegate_->getWidget() : nullptr;
}

void RootView::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

void RootView::handleMouse(NativeEvent & e)
{
    int flags = Helper::ToFlagsForEvent();

    //暂时不管XBUTTON
    EventType et = kET_COUNT;
    MouseButton mb = kMB_NONE;

    Helper::ToEMForMouse(e.msg, et, mb);
    auto & lparam = e.lparam;
    Point p(static_cast<Scalar>(GET_X_LPARAM(lparam)),
        static_cast<Scalar>(GET_Y_LPARAM(lparam)));
    MouseEvent me(et, mb, this, p, p, flags);
    me.setUserData(&e);
    mouse_.handleEvent(me);
}

void RootView::handleKey(NativeEvent & e)
{
    //处理键盘事件
    View * focus = focus_.current();
    EventType type = kET_COUNT;
    auto & msg = e.msg;
    if (WM_CHAR == msg)
        type = kET_KEY_PRESS;
    else if (WM_KEYDOWN == msg)
        type = kET_KEY_DOWN;
    else if (WM_KEYUP == msg)
        type = kET_KEY_UP;
    else
        return;

    bool handle = false;

    KeyEvent ke(type, focus, (KeyboardCode)e.wparam, *(KeyState *)(&e.lparam), 
                Helper::ToFlagsForEvent());
    ke.setUserData(&e);
    if (kET_KEY_PRESS != type)
    {//非字符
        //焦点管理器会询问是否skip
        handle = focus_.handleKeyEvent(ke);

        bool skip = false;
        if (focus)
            skip = focus->skipDefaultKeyEventProcessing(ke);
        if (!handle && !skip)
        {//焦点管理器不处理 看快捷键管理器是否处理
            handle = accelerators_.process(Accelerator::make(ke));
        }
    }
    if (!handle && focus)
    {//都不处理
        EventDispatcher::Push(ke);
        handle = true;
    }
}

void RootView::handleFocus(NativeEvent & e)
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
}

void RootView::handleComposition(NativeEvent & e)
{
    View * focus = focus_.current();
    if (!focus)
        return;
    auto & msg = e.msg;
    EventType type = kET_COUNT;
    if (WM_IME_STARTCOMPOSITION == msg)
        type = kET_COMPOSITION_START;
    else if (WM_IME_COMPOSITION == msg)
        type = kET_COMPOSITION_UPDATE;
    else if (WM_IME_ENDCOMPOSITION == msg)
        type = kET_COMPOSITION_END;
    else
        return;

    CompositionEvent ce(type, focus);
    ce.setUserData(&e);
    EventDispatcher::Push(ce);
}

void RootView::handleWheel(NativeEvent & e)
{
    int flags = Helper::ToFlagsForEvent();
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
        mouse_.handleEvent(we);
    }
}

bool RootView::isVisible() const
{
    return visible();
}

RootView * RootView::getRoot()
{
    return this;
}

const char * RootView::getClassName() const
{
    return kClassRootView;
}

void RootView::draw()
{
    AdjustPixmap();
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
    View::draw(canvas, rect);
}

const Rect & RootView::getDirtyRect() const
{
    return dirty_;
}

bool RootView::isDirty() const
{
    return !dirty_.isEmpty();
}

Surface & RootView::getBackBuffer()
{
    AdjustPixmap();
    return back_buffer_;
}

void RootView::onDraw(SkCanvas & canvas, const Rect & inval)
{  
    SkPaint paint;
    paint.setColor(0);
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    canvas.drawPaint(paint);
}

bool RootView::notifyInval(const Rect & inval)
{
    dirty_.join(inval);
    delegate_ ? delegate_->invalidateRect(inval) : 0;
    return true;
}

bool RootView::notifyAddHierarchy(View * n)
{
    return true;
}

bool RootView::notifyRemoveHierarchy(View * n)
{
    focus_.removed(n);
    mouse_.removed(n);
    return true;
}

bool RootView::notifyVisibleChanged(View * n)
{
    if (n->visible())
        return true;
    focus_.shiftIfNecessary();
    mouse_.shiftIfNecessary();
    return true;
}

bool RootView::notifySetFocus(View * n)
{
    delegate_ ? delegate_->requestFocus() : 0;
    if (has_focus_)
        focus_.shift(n);
    return true;
}

bool RootView::notifyChangeCursor(Cursor cursor)
{
    delegate_ ? delegate_->changeCursor(cursor) : 0;
    return true;
}

void RootView::onVisibleChanged(View * n)
{
    assert(this == n);
    if (n->visible())
        return;
    focus_.shiftIfNecessary();
    mouse_.shiftIfNecessary();
}

void RootView::AdjustPixmap()
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
        back_buffer_.free();
        if (w && h)
        {
            back_buffer_.allocate(w, h);
            device_ = Device::Create(back_buffer_);
        }            
    }
}


}