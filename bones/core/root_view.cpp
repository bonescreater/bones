
#include "root_view.h"
#include "rect.h"
#include "event.h"
#include "event_dispatcher.h"
#include "helper.h"
#include "SkCanvas.h"
#include "SkBitmap.h"

namespace bones
{

RootView::RootView()
:delegate_(nullptr), mouse_(this), focus_(this), color_(0),
opacity_(1.0f)
{
    ;
}

RootView::~RootView()
{
    ;
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
    SkCanvas canvas(Helper::ToSkBitmap(back_buffer_));
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

Pixmap & RootView::getPixmap()
{
    AdjustPixmap();
    return back_buffer_;
}


void RootView::handleEvent(MouseEvent & e)
{
    mouse_.handleEvent(e);
}

void RootView::handleEvent(KeyEvent & e)
{
    //处理键盘事件
    View * focus = focus_.current();
    bool handle = false;
    if (focus && focus->skipDefaultKeyEventProcessing(e))
    {//
        handle = focus_.handleKeyEvent(e);
    }
    if (!handle)
    {//焦点管理器不处理 看快捷键管理器是否处理
        handle = accelerators_.process(Accelerator::make(e));
    }
    if (!handle && focus)
    {//都不处理
        KeyEvent focus_ke(e.type(), focus, e.key(), e.getFlags());
        EventDispatcher dispatcher;
        EventDispatcher::Path path;
        EventDispatcher::getPath(focus_ke.target(), path);
        dispatcher.run(focus_ke, path);
        handle = true;
    }
}

void RootView::onDraw(SkCanvas & canvas)
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
    if (w != back_buffer_.width() || h != back_buffer_.height())
    {
        back_buffer_.free();
        if (w && h)
            back_buffer_.allocate(w, h);
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