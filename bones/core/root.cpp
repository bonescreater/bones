
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
#include "core_imp.h"
#include "animation_manager.h"
#include "res_manager.h"

namespace bones
{

Root::Root()
:mouse_(this), focus_(this), device_(nullptr), delegate_(nullptr),
widget_(NULL), color_(0), 
caret_show_(false), caret_display_(false), caret_ani_(nullptr),
focus_caret_display_(false)
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

    return ( kClassWebView != focus->getClassName() && kClassInput != focus->getClassName())
        || msg != WM_IME_COMPOSITION;
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

void Root::restoreCaret()
{
    notifyDestroyCaret(focus_.current());
}

void Root::restoreCursor()
{
    setCursor(Core::GetResManager()->getCursor("arrow"));
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

    if (!rect.intersect(local_bounds))
        return;

    if (rect.isEmpty())
        return;

    if (!back_buffer_.isValid())
        return;
    if (!device_)
        return;
    SkCanvas canvas(device_);
    View::draw(canvas, rect, 1.0f);
    //绘制光标
    drawCaret(canvas, rect);
}

void Root::drawCaret(SkCanvas & canvas, const Rect & inval)
{
    if (!caret_display_ && !focus_caret_display_)//光标不显示
        return;
    if (focus_caret_display_)
        focus_caret_display_ = false;

    //无效区跟光标有交集
    Rect caret = getCaretRect();
    if (!caret.intersect(inval))
        return;
    if (caret.isEmpty())
        return;
    View * focus = focus_.current();
    if (focus)
    {
        float opacity = focus->getOpacity();
        View * parent = focus->parent();
        while (parent)
        {
            float popa = parent->getOpacity();
            opacity = opacity > popa ? popa : opacity;
            parent = parent->parent();
        }

        auto count = canvas.save();
        canvas.clipRect(Helper::ToSkRect(caret));
        canvas.translate(caret.left(), caret.top());
        //只有当前焦点才能绘制光标焦点切换同样会销毁旧光标
        if (focus)
            focus->onDrawCaret(canvas, caret, caret_size_, opacity);
        canvas.restoreToCount(count);
    }

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
    if (!inval.isEmpty())
    {
        dirty_.join(inval);
        delegate_ ? delegate_->invalidRect(this, inval) : 0;
    }
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
//caret打算自绘
bool Root::notifyShowCaret(View * n, bool show)
{//只有当前焦点
    if (n && n == focus_.current() && caret_show_ != show && caret_ani_)
    {
        caret_show_ = show;
        if (caret_show_)//定时器继续运行
        {
            Core::GetAnimationManager()->resume(caret_ani_);
        }
        else
        {//暂停定时器 隐藏光标显示
            Core::GetAnimationManager()->pause(caret_ani_);
            caret_display_ = false;
            inval(getCaretRect());
        }
    }
    return true;
}

bool Root::notifyChangeCaretPos(View * n, const Point & pt)
{
    if (n && n == focus_.current() && caret_ani_)
    {
        if (caret_loc_ != pt)
        {
            Rect caret = getCaretRect();
            caret_loc_ = pt;
            if (caret_show_)
            {
                focus_caret_display_ = true;
                caret.join(getCaretRect());
                inval(caret);
            }
        }
    }
    return true;
}

bool Root::notifyChangeCaretSize(View * n, const Size & size)
{
    if (n && n == focus_.current() && caret_ani_)
    {
        if (caret_size_ != size)
        {
            Rect caret = getCaretRect();
            caret_size_ = size;
            if (caret_show_)
            {
                focus_caret_display_ = true;
                caret.join(getCaretRect());
                inval(caret);
            }
        }
    }
    return true;
}

bool Root::notifyCreateCaret(View * n)
{
    if (n && n == focus_.current())
    {
        assert(caret_ani_ == nullptr);
        if (caret_ani_)
            destroyCaret();
        //创建一个光标
        caret_ani_ = new Animation(this, 400, -1);
        caret_ani_->bind(BONES_CLASS_CALLBACK_3(&Root::onCaretRun, this));
        caret_ani_->bind(Animation::kStop,
            BONES_CLASS_CALLBACK_2(&Root::onCaretStop, this));
        caret_ani_->bind(Animation::kStart,
            BONES_CLASS_CALLBACK_2(&Root::onCaretStart, this));

        Core::GetAnimationManager()->add(caret_ani_);
        //光标闪烁暂停
        Core::GetAnimationManager()->pause(caret_ani_);
    }
    return true;
}

bool Root::notifyDestroyCaret(View * n)
{//任何view都可以销毁光标 以便 写控件的时候偷懒
    //但是标准的写法还是应该create和destroy对应
    //如果光标存在则移除光标
    if (caret_ani_)
    {
        Core::GetAnimationManager()->remove(caret_ani_, false);
        caret_ani_->release();
        caret_ani_ = nullptr;
    }
    return true;
}

Root::DelegateBase * Root::delegate()
{
    return delegate_;
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


Rect Root::getCaretRect()
{
    return Rect::MakeXYWH(caret_loc_.x(), caret_loc_.y(), 
        caret_size_.width(), caret_size_.height());

}

void Root::onCaretRun(Animation * sender, View * target, float progress)
{//光标启用后 会自动闪烁
    caret_display_ = !caret_display_;
    if (focus_caret_display_)
    {
        focus_caret_display_ = false;
        caret_display_ = true;
    }
    inval(getCaretRect());
}

void Root::onCaretStop(Animation * sender, View * target)
{//销毁光标
    Rect rect = getCaretRect();
    caret_loc_.set(0, 0);
    caret_size_.set(0, 0);
    caret_show_ = false;
    caret_display_ = false;
    inval(rect);
}

void Root::onCaretStart(Animation * sender, View * target)
{
    caret_loc_.set(0, 0);
    caret_size_.set(0, 0);
    caret_show_ = false;
    caret_display_ = false;
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