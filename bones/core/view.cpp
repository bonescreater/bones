#include "view.h"
#include "rect.h"
#include "helper.h"
#include "SkCanvas.h"

namespace bones
{
static int kInvalidTag = -1;

///*iterator front to back*/
//class F2BIter
//{
//public:
//    F2BIter(const View * parent)
//    {
//        if (parent)
//            first_child_ = parent->first_child_;
//
//        child_ = first_child_;
//    }
//    RefPtr<View> next()
//    {
//        RefPtr<View> cur = child_;
//        if (child_)
//        {
//            child_ = child_->next_sibling_;
//            if (child_ == first_child_)
//                child_.reset();
//        }
//        return cur;
//    }
//private:
//    RefPtr<View> first_child_;
//    RefPtr<View> child_;
//};
//
///*iterator back to front*/
//class B2FIter
//{
//public:
//    B2FIter(const View* parent)
//    {
//        if (parent)
//            first_child_ = parent->first_child_;
//
//        if (first_child_)
//            child_ = first_child_->prev_sibling_;
//    }
//    RefPtr<View> next()
//    {
//        RefPtr<View> cur = child_;
//        if (child_)
//        {          
//            if (child_ == first_child_)
//                child_.reset();
//            else 
//                child_ = child_->prev_sibling_;
//        }
//        return cur;
//    }
//private:
//    RefPtr<View> first_child_;
//    RefPtr<View> child_;
//};

View::View() :tag_(kInvalidTag), group_id_(-1), children_count_(0)
{
    flag_.focusable = true;
    flag_.visible = true;
    flag_.clip = true;
}

View::~View()
{
    ;
}

void View::setLoc(Scalar x, Scalar y)
{
    setLoc(Point::Make(x, y));
    
}

void View::setSize(Scalar w, Scalar h)
{
    setSize(Size::Make(w, h));
}

void View::setLoc(const Point & pt)
{
    if (loc_ != pt)
    {
        Point changing = pt;
        inval();
        onPositionChanging(changing);
        loc_ = changing;
        onPositionChanged();
        inval();
    }
}

void View::setSize(const Size & size)
{
    if (size_ != size)
    {
        Size changing = size;
        inval();
        onSizeChanging(changing);
        size_ = size;
        onSizeChanged();
        inval();
    }
}

void View::setLeft(Scalar left)
{
    setLoc(left, getTop());
}

void View::setTop(Scalar top)
{
    setLoc(getLeft(), top);
}

void View::setWidth(Scalar width)
{
    setSize(width, getHeight());
}

void View::setHeight(Scalar height)
{
    setSize(getWidth(), height);
}

const Size & View::getSize() const
{
    return size_;
}

Scalar View::getLeft() const
{
    return loc_.x();
}

Scalar View::getTop() const
{
    return loc_.y();
}

Scalar View::getWidth() const
{
    return size_.width();
}

Scalar View::getHeight() const
{
    return size_.height();
}

void View::setClip(bool clip)
{
    flag_.clip = clip;
}

void View::setVisible(bool vis)
{
    if (vis != visible())
    {
        flag_.visible = vis;
        propagateVisibleNotifications();
    }
    
}

void View::setFocusable(bool focusable)
{
    flag_.focusable = focusable;
}

bool View::visible() const
{
    return flag_.visible;
}

bool View::focusable() const
{
    return flag_.focusable;
}

bool View::clip() const
{
    return flag_.clip;
}

bool View::isVisible() const
{
    return visible() && parent_ ? parent_->visible() : false;
}

bool View::isFocusable() const
{
    return focusable() && isVisible();
}

bool View::isClip() const
{
    return clip();
}

bool View::process(const Accelerator & accelerator)
{
    return false;
}

bool View::canProcess() const
{
    return true;
}

const char * View::getClassName() const
{
    return kClassView;
}

void View::inval()
{
    notifyInval(Rect::MakeXYWH(0, 0, getWidth(), getHeight()));
}

size_t View::getChildCount() const
{
    return children_count_;
}

void View::attachChildToFront(View * child)
{
    attachChildAt(child, 0);
}

void View::attachChildToBack(View * child)
{
    attachChildAt(child, getChildCount());
}

void View::attachChildAt(View * child, size_t index)
{
    if (!child || child == this)
        return;

    //超出范围
    if (getChildCount() < index)
        return;
    //环形链表
    RefPtr<View> ch;
    ch.reset(child);
    ch->detachFromParent();

    if (first_child_)
    {//已经有子了
        RefPtr<View> old = first_child_;
        if (index < getChildCount())
        {//在中间插入
            for (size_t i = index; i > 0; i--)
                old = old->next_sibling_;
        }
        ch->next_sibling_ = old;
        ch->prev_sibling_ = old->prev_sibling_;
        old->prev_sibling_->next_sibling_ = ch;
        old->prev_sibling_ = ch;

        if (0 == index)
            first_child_ = ch;
    }
    else
    {
        ch->next_sibling_ = ch;
        ch->prev_sibling_ = ch;
        first_child_ = ch;
    }

    insertFocusSblings(child, index);

    children_count_++;
    ch->parent_.reset(this);
    child->propagateAddNotifications();
    if (ch->isVisible())
        ch->inval();
}

void View::detachChildren()
{
    while (children_count_)
        detachChild(first_child_.get());
}

void View::detachFromParent()
{
    if (parent_)
        parent_->detachChild(this);
}

void View::detachChild(View * child)
{
    if (!child || child == this)
        return;

    if (child->parent() != this)
    {
        assert(0);
        return;
    }

    RefPtr<View> ch;
    ch.reset(child);

    eraseFocusSblings(child);

    if (ch->isVisible())
        ch->inval();

    if (1 == children_count_)
    {//只有1个子
        first_child_.reset();
    }
    else
    {//多个子
        if (ch == first_child_)//删除头
            first_child_ = ch->next_sibling_;

        ch->prev_sibling_->next_sibling_ = ch->next_sibling_;
        ch->next_sibling_->prev_sibling_ = ch->prev_sibling_;
    }
    ch->prev_sibling_.reset();
    ch->next_sibling_.reset();

    children_count_--;
    ch->propagateRemoveNotifications();
    ch->parent_.reset();
}

void View::recursiveDetachChildren()
{
    
    while (auto child = getFirstChild())
        child->recursiveDetachChildren();

    assert(children_count_ == 0 && !first_child_);
    detachFromParent();
}

View * View::getChildAt(size_t index) const
{
    if (index >= getChildCount())
        return nullptr;
    auto child = first_child_;

    for (; index > 0; index--)
        child = child->next_sibling_;

    return child.get();
}

View * View::getFirstChild() const
{
    return first_child_.get();
}

View * View::getLastChild() const
{
    if (first_child_)
        return first_child_->prev_sibling_.get();
    return nullptr;
}

View * View::getNextSibling() const
{
    View * child = nullptr;
    if (parent_ && parent_->first_child_ != next_sibling_)
        child = next_sibling_.get();
    return child;
}

View * View::getPrevSibling() const
{
    View * child = nullptr;
    if (parent_ && parent_->first_child_.get() != this)
        child = prev_sibling_.get();
    return child;
}

View * View::parent() const
{
    return parent_.get();
}

RootView * View::getRoot()
{
    if (parent_)
        return parent_->getRoot();
    return nullptr;
}

void View::dispatch(Event & e)
{
    switch (e.type())
    {
    case kET_MOUSE_ENTER:
        onMouseEnter(*MouseEvent::From(e));
        break;
    case kET_MOUSE_MOVE:
        onMouseMove(*MouseEvent::From(e));
        break;
    case kET_MOUSE_DOWN:
        onMouseDown(*MouseEvent::From(e));
        break;
    case kET_MOUSE_UP:
        onMouseUp(*MouseEvent::From(e));
        break;
    case kET_MOUSE_CLICK:
        onMouseClick(*MouseEvent::From(e));
        break;
    case kET_MOUSE_DCLICK:
        onMouseDClick(*MouseEvent::From(e));
        break;
    case kET_MOUSE_LEAVE:
        onMouseLeave(*MouseEvent::From(e));
        break;
    case kET_KEY_DOWN:
        onKeyDown(*KeyEvent::From(e));
        break;
    case kET_KEY_UP:
        onKeyUp(*KeyEvent::From(e));
        break;
    case kET_FOCUS_OUT:
        onFocusOut(*FocusEvent::From(e));
        break;
    case kET_FOCUS_IN:
        onFocusIn(*FocusEvent::From(e));
        break;
    case kET_BLUR:
        onBlur(*FocusEvent::From(e));
        break;
    case kET_FOCUS:
        onFocus(*FocusEvent::From(e));
        break;
    case kET_MOUSE_WHEEL:
        onWheel(*WheelEvent::From(e));
        break;
    default:
        break;
    }
}

void View::getGlobalBounds(Rect & rect) const
{
    const View * tmp = this;
    Scalar left = 0;
    Scalar top = 0;
    while (tmp->parent())
    {
        left += tmp->getLeft();
        top += tmp->getTop();
        tmp = tmp->parent();
    }
    rect.setXYWH(left, top, size_.width(), size_.height());
}

void View::getLocalBounds(Rect & rect) const
{
    rect.setXYWH(0, 0, size_.width(), size_.height());
}

Point View::mapToGlobal(const Point & loc) const
{
    Rect r;
    getGlobalBounds(r);
    return Point::Make(loc.x() + r.left(), loc.y() + r.top());
}

Point View::mapToLocal(const Point & global) const
{
    Rect r;
    getGlobalBounds(r);
    return Point::Make(global.x() - r.left(), global.y() - r.top());
}

bool View::notifyRemoveHierarchy(View * n)
{
    if (parent_)
        return parent_->notifyRemoveHierarchy(n);
    return true;
}

bool View::notifyAddHierarchy(View * n)
{
    if (parent_)
        return parent_->notifyAddHierarchy(n);
    return true;
}

bool View::notifyVisibleChanged(View * n)
{
    if (parent_)
        return parent_->notifyVisibleChanged(n);
    return true;

}

bool View::notifyInval(const Rect & inval)
{
    if (parent_)
    {
        Rect parent_inval(inval);
        parent_inval.offset(loc_.x(), loc_.y());
        return parent_->notifyInval(parent_inval);
    }
    return true;
}

bool View::notifySetFocus(View * n)
{
    if (parent_)
        return parent_->notifySetFocus(n);
    return true;
}

bool View::notifyRegAccelerator(View * n, const Accelerator & accelerator)
{
    if (parent_)
        return parent_->notifyRegAccelerator(n, accelerator);
    return true;
}

bool View::notifyUnregAccelerator(View * n, const Accelerator & accelerator)
{
    if (parent_)
        return parent_->notifyUnregAccelerator(n, accelerator);
    return true;
}

bool View::notifyChangeCursor(Cursor cursor)
{
    if (parent_)
        return parent_->notifyChangeCursor(cursor);
    return true;
}

void View::blink()
{
    onBlink();
    auto child = getFirstChild();
    while (child)
    {
        child->blink();
        child = child->getNextSibling();
    }
}

void View::draw(SkCanvas & canvas, const Rect & inval)
{
    if (!visible() || inval.isEmpty())
        return;

    bool c = clip();
    //无效区与自己有交集才绘制自己
    Rect local_bounds;
    getLocalBounds(local_bounds);
    Rect intersect_bounds(local_bounds);
    if (intersect_bounds.intersect(inval) && !intersect_bounds.isEmpty())
    {
        //绘制自己的时候 不能超出矩形
        auto count = canvas.save();
        canvas.clipRect(Helper::ToSkRect(intersect_bounds));
        onDraw(canvas);
        canvas.restoreToCount(count);
    }
    else if (c)
    {//无交集 且自己是裁剪子的 必然跟子孙无交集直接返回
        return;
    }
    //准备绘制子

    //自己是裁剪子的 所有的子是不能超出自己范围的
    int count = 0;
    if (c)
    {
        count = canvas.save();
        canvas.clipRect(Helper::ToSkRect(local_bounds));
    }

    auto child = getFirstChild();
    while (child)
    {
        Rect child_inval(inval);
        child_inval.offset(-child->getLeft(), -child->getTop());
        auto child_count = canvas.save();
        canvas.translate(child->getLeft(), child->getTop());
        child->draw(canvas, child_inval);
        canvas.restoreToCount(child_count);

        child = child->getNextSibling();
    }
    if (c)
        canvas.restoreToCount(count);
}



bool View::onHitTest(const Point & pt)
{
    return true;
}

void View::onBlink()
{

}

void View::onTrigger(int tag, uint32_t interval)
{

}

void View::onDraw(SkCanvas & canvas)
{
    ;
}

void View::onSizeChanging(Size & size)
{
    ;
}

void View::onSizeChanged()
{
    ;
}

void View::onPositionChanging(Point & pt)
{
    ;
}

void View::onPositionChanged()
{
    ;
}

void View::onVisibleChanged(View * start)
{
    ;
}

void View::onAddHierarchy(View * start)
{
    ;
}

void View::onRemoveHierarchy(View * start)
{
    ;
}

void View::onMouseEnter(MouseEvent & e)
{
    ;
}

void View::onMouseMove(MouseEvent & e)
{
    ;
}

void View::onMouseDown(MouseEvent & e)
{
    ;
}

void View::onMouseUp(MouseEvent & e)
{
    ;
}

void View::onMouseClick(MouseEvent & e)
{
    ;
}

void View::onMouseDClick(MouseEvent & e)
{
    ;
}

void View::onMouseLeave(MouseEvent & e)
{
    ;
}

void View::onKeyDown(KeyEvent & e)
{
    ;
}

void View::onKeyUp(KeyEvent & e)
{
    ;
}

void View::onFocusOut(FocusEvent & e)
{
    ;
}

void View::onFocusIn(FocusEvent & e)
{
    ;
}

void View::onBlur(FocusEvent & e)
{
    ;
}

void View::onFocus(FocusEvent & e)
{
    ;
}

void View::onWheel(WheelEvent & e)
{
    ;
}

View * View::hitTest(const Point & pt)
{
    //被隐藏 子孙也不可见
    if (!visible())
        return nullptr;
    //clip 且 pt不在自身范围内
    bool contain = contains(pt);
    if (clip() && !contain)
        return nullptr;

    View * target = nullptr;

    auto child = getLastChild();
    while (child)
    {
        Point child_pt(pt.x() - child->getLeft(), pt.y() - child->getTop());
        target = child->hitTest(child_pt);
        if (target)
            return target;
        child = child->getPrevSibling();
    }

    if (contain && onHitTest(pt))
        target = this;
    return target;
}

bool View::contains(const Point & pt) const
{
    Rect bounds = Rect::MakeXYWH(0, 0, size_.width(), size_.height());
    return bounds.contains(pt.x(), pt.y());
}

bool View::contains(View * v) const
{
    auto tmp = v;
    while (tmp)
    {
        if (tmp == this)
            return true;
        tmp = tmp->parent();
    }
    return false;
}

int View::getGroup() const
{
    return group_id_;
}

void View::getViewsInGroup(int gid, Views & views) const
{
    if (group_id_ == gid)
        views.push_back(const_cast<View *>(this));

    auto child = getFirstChild();
    while (child)
    {
        child->getViewsInGroup(gid, views);
        child = child->getNextSibling();
    }
}

View * View::getSelectedForGroup(int group_id) const
{
    Views vs;

    const View * root = this;
    while (root->parent())
        root = root->parent();

    root->getViewsInGroup(group_id, vs);
    return vs.empty() ? nullptr : vs[0];
}

View * View::getPrevFocusable() const
{
    return getPrevSibling();
}

View * View::getNextFocusable() const
{
    return getNextSibling();
}

void View::requestFocus()
{
    if (isFocusable())
        notifySetFocus(this);
}

void View::setCursor(Cursor cursor)
{
    notifyChangeCursor(cursor);
}

bool View::isGroupFocusTraversable() const
{
    return true;
}

bool View::skipDefaultKeyEventProcessing(const KeyEvent & ke)
{
    return false;
}

void View::insertFocusSblings(View * v, int index)
{
    //int count = children_count_;
    //if (count == 0)
    //{//没有子
    //    v->next_focusable_.reset(nullptr);
    //    v->prev_focusable_.reset(nullptr);
    //}
    //else
    //{//有子
    //    if (index == count)
    //    {//子的位置在末尾
    //        View * last_focusable = nullptr;
    //        for (auto iter = children_.begin(); iter != children_.end(); ++iter)
    //        {
    //            if (!(*iter)->next_focusable_)
    //            {
    //                last_focusable = iter->get();
    //                break;
    //            }
    //        }
    //        assert(last_focusable);
    //        last_focusable->next_focusable_.reset(v);
    //        v->next_focusable_.reset(nullptr);
    //        v->prev_focusable_.reset(last_focusable);
    //    }
    //    else
    //    {//子的位置在中间
    //        auto prev = children_[index]->prev_focusable_;
    //        v->prev_focusable_ = prev;
    //        v->next_focusable_ = children_[index];
    //        if (prev)
    //            prev->next_focusable_.reset(v);
    //        children_[index]->prev_focusable_.reset(v);
    //    }
    //}
}

void View::eraseFocusSblings(View * v)
{
    View * next = v->next_focusable_.get();
    View * prev = v->prev_focusable_.get();
    if (prev)
        prev->next_focusable_.reset(next);
    if (next)
        next->prev_focusable_.reset(prev);

    v->next_focusable_.reset();
    v->prev_focusable_.reset();
}

void View::propagateRemoveNotifications()
{
    notifyRemoveHierarchy(this);
    recursiveRemoveNotifications(this);
}

void View::recursiveRemoveNotifications(View * start)
{
    onRemoveHierarchy(start);

    auto child = getFirstChild();
    while (child)
    {
        child->recursiveRemoveNotifications(start);
        child = child->getNextSibling();
    }
}

void View::propagateAddNotifications()
{
    //通知父
    notifyAddHierarchy(this);
    recursiveAddNotifications(this);
}

void View::recursiveAddNotifications(View * start)
{
    onAddHierarchy(start);
    auto child = getFirstChild();
    while (child)
    {
        child->recursiveAddNotifications(start);
        child = child->getNextSibling();
    }
}

void View::propagateVisibleNotifications()
{
    //通知父
    notifyVisibleChanged(this);
    //通知子
    recursiveVisibleNotifications(this);
}

void View::recursiveVisibleNotifications(View * start)
{
    onVisibleChanged(start);
    //通知子
    auto child = getFirstChild();
    while (child)
    {
        child->recursiveVisibleNotifications(start);
        child = child->getNextSibling();
    }
}


}