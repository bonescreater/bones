
#include "focus_controller.h"
#include "event_dispatcher.h"
#include "event.h"
#include "root.h"


namespace bones
{

FocusController::FocusController(Root * root)
:root_(root), arrow_key_traversal_enabled_(false),
reason_(kTraversal)
{

}

FocusController::~FocusController()
{
    ;
}

bool FocusController::handleKeyEvent(const KeyEvent & ke)
{
    auto key_code = ke.key();

    if (kET_KEY_DOWN != ke.type())
        return false;

    if (current_ && current_->skipDefaultKeyEventProcessing(ke))
        return false;

    if (isTabTraversalKeyEvent(ke))
    {
        advanceFocus(ke.isShiftDown());
        return true;
    }

    if (arrow_key_traversal_enabled_ && processArrowKeyTraversal(ke))
        return true;

    if (current_ && current_->group() != -1 && isArrowKeyEvent(ke))
    {
        bool reverse = (key_code == kVKEY_LEFT || key_code == kVKEY_UP);
        advanceFocusInGroup(reverse);
        return true;
    }

    return false;
}

void FocusController::removed(View * n)
{
    if (n && n->contains(current_.get()))
        setFocusWithReason(nullptr, kTraversal);
}

void FocusController::shiftIfNecessary()
{
    if (current_ && !current_->isFocusable())
        advanceFocus(false);
}

void FocusController::shift(View * n)
{
    if (n == root_)
        n = nullptr;
    //当view不可接受焦点时 不切换焦点
    //为了在滚动条上点击时 不切换编辑框的焦点这样编辑框继续显示光标
    if (n && !n->isFocusable())
        return;
    //View * tmp = n;
    //while (tmp)
    //{
    //    if (tmp->isFocusable())
    //        break;
    //    tmp = tmp->parent();
    //}
    setFocusWithReason(n, kDirectChange);
}

View * FocusController::current() const
{
    return current_.get();
}

bool FocusController::isTabTraversalKeyEvent(const KeyEvent & ke)
{
    return ke.key() == kVKEY_TAB && !ke.isControlDown();
}

bool FocusController::isArrowKeyEvent(const KeyEvent & ke)
{
    auto key_code = ke.key();
    return (!ke.isShiftDown() && !ke.isControlDown() && !ke.isAltDown()) ||
        (key_code == kVKEY_LEFT || key_code == kVKEY_UP ||
        key_code == kVKEY_RIGHT|| key_code == kVKEY_DOWN);
}

void FocusController::advanceFocus(bool reverse)
{
    clearFocusIfNecessary();
    View * start = current_.get();
    View * newly = findNextFocusable(start, reverse);

    if (!newly)
    {
        if (start != root_)
        {
            start = root_;
            newly = findNextFocusable(start, reverse);
        }
    }
    //newly 可能为空
    if (newly != current_.get())
        setFocusWithReason(newly, kTraversal);
}

void FocusController::advanceFocusInGroup(bool reverse)
{
    clearFocusIfNecessary();
    if (!current_ || current_->group() == -1)
        return;

    View::Views views;
    current_->parent()->getViewsInGroup(current_->group(), views);
    auto iter = std::find(views.begin(), views.end(), current_.get());
    if (iter == views.end())
        return;

    int index = static_cast<int>(iter - views.begin());
    int max_size = static_cast<int>(views.size());

    index += reverse ? -1 : 1;
    if (index < 0)
        index = max_size - 1;
    else if (index >= max_size)
        index = 0;

    setFocusWithReason(views[index], kTraversal);
}

void FocusController::clearFocusIfNecessary()
{
    if (current_ && !root_->contains(current_.get()))
        setFocusWithReason(nullptr, kTraversal);
}

void FocusController::setFocusWithReason(View * view, FocusChangeReason reason)
{
    if (current_.get() == view)
        return;
    EventDispatcher dispatcher;
    EventDispatcher::Path last_path, newly_path;

    bool tab = (reason == kTraversal);

    View *last = current_.get();

    EventDispatcher::GetPath(last, last_path);
    EventDispatcher::GetPath(view, newly_path);
    if (last)
        dispatcher.run(FocusEvent(kET_FOCUS_OUT, last, tab), last_path);
    if (view)
        dispatcher.run(FocusEvent(kET_FOCUS_IN, view, tab), newly_path);

    root_->restoreCaret();
    current_.reset(view);
    reason_ = reason;


    if (last)
        dispatcher.run(FocusEvent(kET_BLUR, last, tab), last_path);
    if (view)
        dispatcher.run(FocusEvent(kET_FOCUS, view, tab), newly_path);

}

bool FocusController::isFocusable(View * v)
{
    return v && v->isFocusable();
}

bool FocusController::isFocusableCandidate(View * v, int skip_group_id)
{
    return isFocusable(v) &&
        (v->isGroupFocusTraversable() || skip_group_id == -1 ||
        v->group() != skip_group_id);
}

View * FocusController::findSelectedForGroup(View * v)
{
    if (!v)
        return nullptr;
    if (v->isGroupFocusTraversable() || v->group() == -1)
        return v;

    View * selected = v->getSelectedForGroup(v->group());
    if (selected)
        return selected;

    return v;
}

bool FocusController::processArrowKeyTraversal(const KeyEvent & ke)
{
    auto key_code = ke.key();
    if (key_code == kVKEY_LEFT || key_code == kVKEY_UP)
    {
        advanceFocus(true);
        return true;
    }

    if (key_code == kVKEY_RIGHT || key_code == kVKEY_DOWN)
    {
        advanceFocus(false);
        return true;
    }
    return false;
}

View * FocusController::findNextFocusable(View * start, bool reverse)
{
    if (!start)
        return nullptr;

    auto root = start->getRoot();
    assert(root == root_);

    bool check_start = false;
    if (start == root)
    {
        if (root->getChildCount() == 0)
            return nullptr;

        check_start = true;
        if (reverse)
        {
            while (start->getChildCount() > 0)
            {
                start = start->getFirstChild();
                while (start->getNextFocusable())
                    start = start->getNextFocusable();
            }
        }
        else
            start = root->getFirstChild();
    }

    int start_group = start->group();

    View * newly = nullptr;
    if (!reverse)
        newly = findNextFocusable(start, check_start, true, true, start_group);
    else
    {
        bool can_go_down = !isFocusable(start);
        newly = findPrevFocusable(start, check_start, true, can_go_down, start_group);
    }

    if (newly == root)
        newly = nullptr;
    return newly;
}

View * FocusController::findNextFocusable(View * start,
    bool check_start,
    bool can_go_up,
    bool can_go_down,
    int skip_group_id)
{
    //1.看自己是否可以获取焦点
    if (check_start)
    {
        if (isFocusableCandidate(start, skip_group_id))
        {
            auto v = findSelectedForGroup(start);
            if (isFocusable(v))
                return v;
        }
    }

    //2.看自己的子是否可以获取焦点
    if (can_go_down)
    {
        if (start->getChildCount() > 0)
        {//对于子 不需要向上搜索
            auto v = findNextFocusable(
                start->getFirstChild(),
                true, false, true, skip_group_id);

            if (v)
                return v;
        }
    }

    //3.看自己的兄弟是否可以获取焦点
    View * sibling = start->getNextFocusable();
    if (sibling)
    {
        auto v = findNextFocusable(
            sibling,
            true, false, true, skip_group_id);
        if (v)
            return v;
    }

    //4.看自己父的兄是否可以获取焦点
    if (can_go_up)
    {
        auto parent = start->parent();

        while (parent)
        {
            sibling = parent->getNextFocusable();
            if (sibling)
                return findNextFocusable(sibling, true, true, true, skip_group_id);

            parent = parent->parent();
        }
    }

    return nullptr;
}

View * FocusController::findPrevFocusable(View * start,
    bool check_start,
    bool can_go_up,
    bool can_go_down,
    int skip_group_id)
{
    if (can_go_down)
    {
        if (start->getChildCount() > 0)
        {
            int index = static_cast<int>(start->getChildCount()) - 1;
            auto v = findPrevFocusable(
                start->getFirstChild(),
                true, false, true, skip_group_id);
            if (v)
                return v;
        }
    }

    if (check_start && isFocusableCandidate(start, skip_group_id))
    {
        auto gadget = findSelectedForGroup(start);
        if (isFocusable(gadget))
            return gadget;
    }

    //查找兄弟节点
    auto sibling = start->getPrevFocusable();
    if (sibling)
    {
        auto v =  findPrevFocusable(sibling, true, false, true, skip_group_id);
        if (v)
            return v;
    }
        

    //查找父节点时不需要再查子节点
    if (can_go_up)
    {
        auto parent = start->parent();
        if (parent)
            return findPrevFocusable(parent, true, true, false, skip_group_id);
    }

    return nullptr;
}


}