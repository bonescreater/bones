
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
: delegate_(nullptr), mouse_(this), focus_(this), color_(0),
opacity_(1.0f), device_(nullptr)
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

void RootView::setColor(Color color)
{
    color_ = color;
    inval();
}

void RootView::setOpacity(float opacity)
{
    if (opacity_ != opacity)
    {
        opacity_ = opacity;
        inval();
    }   
}

float RootView::getOpacity() const
{
    return opacity_;
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


void RootView::handleEvent(MouseEvent & e)
{
    mouse_.handleEvent(e);
}

void RootView::handleEvent(FocusEvent & e)
{
    if (e.type() == kET_FOCUS)
    {
        has_focus_ = true;
    }
    else if (e.type() == kET_BLUR)
    {
        has_focus_ = false;
        //失去焦点 将内部焦点移除
        focus_.shift(nullptr);
    }
}

void RootView::handleEvent(CompositionEvent & e)
{
    View * focus = focus_.current();
    if (!focus)
        return;
    CompositionEvent ce(e.type(), focus);
    ce.setNativeEvent(e.nativeEvent());
    EventDispatcher dispatcher;
    EventDispatcher::Path path;
    EventDispatcher::getPath(ce.target(), path);
    dispatcher.run(ce, path);
}

void RootView::handleEvent(KeyEvent & e)
{
    //处理键盘事件
    View * focus = focus_.current();
    bool handle = false;
    if (kET_KEY_PRESS != e.type())
    {//非字符
        //焦点管理器会询问是否skip
        handle = focus_.handleKeyEvent(e);

        bool skip = false;
        if (focus)
            skip = focus->skipDefaultKeyEventProcessing(e);
        if (!handle && !skip)
        {//焦点管理器不处理 看快捷键管理器是否处理
            handle = accelerators_.process(Accelerator::make(e));
        }
    }
    if (!handle && focus)
    {//都不处理
        KeyEvent focus_ke(e.type(), focus, e.key(), e.state(), e.getFlags());
        focus_ke.setNativeEvent(e.nativeEvent());
        EventDispatcher dispatcher;
        EventDispatcher::Path path;
        EventDispatcher::getPath(focus_ke.target(), path);
        dispatcher.run(focus_ke, path);
        handle = true;
    }
}

void RootView::onDraw(SkCanvas & canvas, const Rect & inval)
{  
    SkPaint paint;
    paint.setColor(color_);
    paint.setAlpha(ClampAlpha(opacity_, ColorGetA(color_)));
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

//void RootView::injectMouseMove(float x, float y)
//{
    //if (_mouse_x == x && _mouse_y == y)
    //    return;

    //_mouse_x = x;
    //_mouse_y = y;

    //View * target = _mouse.capture();
    //if(!target)
    //    target = hitTest(this, _mouse_x, _mouse_y);

    //_mouse.shiftOver(target);

    //if (target)
    //{
    //    MouseEvent me = MouseEvent::makeMove(target, _mouse_states, _mouse_x, _mouse_y);
    //    EventDispatcher dispatcher;
    //    EventDispatcher::Path path;
    //    EventDispatcher::getPath(target, path);
    //    dispatcher.run(me, path);
    //}
//}

//void RootView::injectMouseDown(MouseButton button)
//{
    //View * target = _mouse.capture();
    //if (!target)
    //    target = hitTest(this, _mouse_x, _mouse_y);

    //if (!target)
    //    return;

    //MouseEvent me = MouseEvent::makeDown(target, button, _mouse_states, _mouse_x, _mouse_y);
    //EventDispatcher dispatcher;
    //EventDispatcher::Path path;
    //EventDispatcher::getPath(target, path);
    //dispatcher.run(me, path);

    //if (!me.canceled())
    //{
    //    if (kMBPrimary == me.button())
    //    {
    //        _mouse.shiftCapture(target);
    //        focus_.shift(target);
    //    }   
    //}
//}

//void RootView::injectMouseUp(MouseButton button)
//{
//    View * target = _mouse.capture();
//    if (!target)
//        target = hitTest(this, _mouse_x, _mouse_y);
//
//    if (!target)
//        return;
//
//    MouseEvent me = MouseEvent::makeUp(target, button, _mouse_states, _mouse_x, _mouse_y);
//    EventDispatcher dispatcher;
//    EventDispatcher::Path path;
//    EventDispatcher::getPath(target, path);
//    dispatcher.run(me, path);
//
//    if (!me.canceled())
//    {
//        if (kMBPrimary == me.button() ||
//            kMBSecondry == me.button())
//        {
//            _mouse.shiftCapture(nullptr);
//            target = hitTest(this, _mouse_x, _mouse_y);
//            _mouse.shiftOver(target);
//        }
//    }
//}

}