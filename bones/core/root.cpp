
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
:mouse_(this), focus_(this), view_device_(nullptr), delegate_(nullptr),
widget_(NULL), color_(0), has_focus_(false), bits_(nullptr), pitch_(0),
caret_show_(false), caret_display_(false), caret_ani_(nullptr),
force_caret_display_(false)
{
    dirty_.flag_ = DirtyRect::kNone;
    wait_dirty_.flag_ = DirtyRect::kNone;
}

Root::~Root()
{
    if (view_device_)
        view_device_->unref();
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

void Root::sendMouse(MouseEvent & e)
{
    if (e.target() != this)
        return;
    mouse_.handleEvent(e);
}

void Root::sendKey(KeyEvent & e)
{
    //处理键盘事件
    if (e.target() != this)
        return;

    bool handle = false;
    View * fv = focus_.current();
    KeyEvent ke(e.type(), fv, e.key(), e.state(), e.system(), e.getFlags());
    if (kET_CHAR != ke.type())
    {//非字符
        bool skip = false;        
        if (fv)
            skip = fv->skipDefaultKeyEventProcessing(ke);
        //焦点管理器会询问是否skip
        if (!skip)
        {//skip == false;
            //焦点管理器
            handle = focus_.handleKeyEvent(ke);
            if (!handle)//焦点管理器不处理 看快捷键管理器是否处理
                handle = accelerators_.process(Accelerator::make(ke));
        }
    }
    if (!handle && fv)
    {//都不处理
        EventDispatcher::Push(ke);
        handle = true;
    }
    
}

void Root::sendFocus(bool focus)
{
    has_focus_ = focus;
    if (!has_focus_)//失去焦点 将内部焦点移除
        focus_.shift(nullptr);
}

bool Root::sendComposition(CompositionEvent &e)
{
    if (e.target() != this)
        return false;

    View * focus = focus_.current();
    if (!focus)
        return true;
    CompositionEvent ce(e.type(), focus, e.index(), e.dbcs(), e.str(), e.cursor());
    EventDispatcher::Push(ce);
    return ( kClassWebView != focus->getClassName())
        || e.type() != kET_COMPOSITION_UPDATE;
}

void Root::sendWheel(WheelEvent & e)
{
    if (e.target() != this)
        return;
    mouse_.handleWheel(e);
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
    notifyChangeCursor(mouse_.over(), kArrow);
}

void Root::shiftFocus(View * focus)
{
    RefPtr<View> prev;
    RefPtr<View> current;

    prev.reset(focus_.current());
    focus_.shift(focus);
    current.reset(focus_.current());
    //焦点切换成功
    if (prev != current)
        delegate_ ? delegate_->shiftFocus(this, prev.get(), current.get()) : 0;
}

void Root::update()
{//root 空闲例程用来提交无效区
    if (DirtyRect::kNone == wait_dirty_.flag_)
        return;
    if (DirtyRect::kView & wait_dirty_.flag_)
        dirty_.flag_ |= DirtyRect::kView;
    if (DirtyRect::kCaret & wait_dirty_.flag_)
        dirty_.flag_ |= DirtyRect::kCaret;
    //合并无效区
    dirty_.rect.join(wait_dirty_.rect);
    wait_dirty_.flag_ = DirtyRect::kNone;
    wait_dirty_.rect.setEmpty();

    delegate_ ? delegate_->invalidRect(this, dirty_.rect) : 0;
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

    DirtyRect backup(dirty_);
    dirty_.flag_ = DirtyRect::kNone;
    dirty_.rect.setEmpty();

    Rect local_bounds;
    getLocalBounds(local_bounds);
    //与root 无交集
    if (!backup.rect.intersect(local_bounds))
        return;

    if (backup.rect.isEmpty())
        return;
    //与root 有交集准备绘制
    //看位图是否已经准备好
    if (!back_buffer_.isValid() || !view_buffer_.isValid())
        return;
    if (!view_device_)
        return;

    SkCanvas view_canvas(view_device_);
    if (backup.flag_ & DirtyRect::kView)
        drawView(view_canvas, backup.rect);    

    //将view的位图绘制到backbuffer上
    SkCanvas canvas(Helper::ToSkBitmap(back_buffer_));
    SkPaint paint;
    paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    canvas.drawBitmap(Helper::ToSkBitmap(view_buffer_), 0, 0, &paint);

    //因为backbuffer已经被刷新了 所以将caret当前的状态绘制上去就行了
    drawCaret(canvas);
}

void Root::drawView(SkCanvas & canvas, const Rect & inval)
{//根据无效区进行绘制
    View::draw(canvas, inval, 1.0f);
}

void Root::drawCaret(SkCanvas & canvas)
{//光标只在root有焦点的情况下才绘制 避免root失去焦点后 进行焦点切换
    if (!caret_display_ && !force_caret_display_ && has_focus_)//光标不显示
        return;
    if (force_caret_display_)
        force_caret_display_ = false;
  
    Rect caret = getCaretRect();
    if (caret.isEmpty())
        return;
    //无效区跟光标有交集
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
            focus->onDrawCaret(canvas, Size::Make(caret.width(), caret.height()), opacity);
        canvas.restoreToCount(count);
    }

}

const Rect & Root::getDirtyRect() const
{
    return dirty_.rect;
}

bool Root::isDirty() const
{
    return (DirtyRect::kNone != dirty_.flag_) && (!dirty_.rect.isEmpty());
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
    invalView(inval);
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
{//只有当前over的指针可以修改cursor
    if (n && n == mouse_.over())
        delegate_ ? delegate_->changeCursor(this, cursor) : 0;
    return true;
}
//caret打算自绘
bool Root::notifyShowCaret(View * n, bool show)
{//只有当前焦点才可以对caret进行操作
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
            force_caret_display_ = false;
            invalCaret();
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
            if (caret_show_)
                invalCaret();

            caret_loc_ = pt;
            if (caret_show_)
            {
                force_caret_display_ = true;
                invalCaret();
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
            if (caret_show_)
                invalCaret();

            caret_size_ = size;
            if (caret_show_)
            {
                force_caret_display_ = true;
                invalCaret();
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
        back_buffer_.unlock();
        back_buffer_.free();
        if (view_device_)
            view_device_->unref();
        view_buffer_.free();

        if (w > 0 && h > 0)
        {
            back_buffer_.allocate(w, h);
            view_buffer_.allocate(w, h);
            view_device_ = Device::Create(view_buffer_);
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
    if (force_caret_display_)
    {
        force_caret_display_ = false;
        caret_display_ = true;
    }
    invalCaret();;
}

void Root::onCaretStop(Animation * sender, View * target)
{//销毁光标
    invalCaret();
    caret_loc_.set(0, 0);
    caret_size_.set(0, 0);
    caret_show_ = false;
    caret_display_ = false;
}

void Root::onCaretStart(Animation * sender, View * target)
{
    caret_loc_.set(0, 0);
    caret_size_.set(0, 0);
    caret_show_ = false;
    caret_display_ = false;
}

void Root::invalView(const Rect & inval)
{
    if (inval.isEmpty())
        return;
    wait_dirty_.flag_ |= DirtyRect::kView;
    wait_dirty_.rect.join(inval);
}

void Root::invalCaret()
{
    auto rect = getCaretRect();
    if (rect.isEmpty())
        return;
    wait_dirty_.flag_ |= DirtyRect::kCaret;
    wait_dirty_.rect.join(rect);
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