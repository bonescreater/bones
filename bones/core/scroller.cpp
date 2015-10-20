#include "scroller.h"
#include "SkCanvas.h"

namespace bones
{


Scroller::Scroller()
:delegate_(nullptr), speed_(4.0f)
{
    container_ = new View;
    attachChildToBack(container_);
    memset(&v_info_, 0, sizeof(v_info_));
    memset(&h_info_, 0, sizeof(h_info_));
}

Scroller::~Scroller()
{
    detachChildren();
    container_->release();
}

void Scroller::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

View * Scroller::getContainer()
{
    return container_;
}

const char * Scroller::getClassName() const
{
    return kClassScroller;
}

Scroller::DelegateBase * Scroller::delegate()
{
    return delegate_;
}

void Scroller::setScrollInfo(Scalar total, bool horizontal)
{
    if (total < 0 )
        return;
    if (!horizontal)
    {
        v_info_.total = total;
        updateVInfo();
    }
    else
    {
        h_info_.total = total;
        updateHInfo();
    }
}

void Scroller::setScrollPos(Scalar cur, bool horizontal)
{
    ScrollInfo * info = nullptr;
    if (!horizontal)
        info = &v_info_;
    else
        info = &h_info_;

    if (cur > info->max_pos)
        cur = info->max_pos;
    if (cur < info->min_pos)
        cur = info->min_pos;
    if (cur != info->cur_pos)
    {
        info->cur_pos = cur;
        if (!horizontal)
            container_->setTop(-info->cur_pos);
        else
            container_->setLeft(-info->cur_pos);

        delegate_ ? delegate_->onScrollPos(this, info->cur_pos, horizontal) : 0;
        inval();
    }
}

void Scroller::setWheelSpeed(float speed)
{
    speed_ = speed;
}

float Scroller::getWheelSpeed() const
{
    return speed_;
}

void Scroller::updateVInfo()
{
    auto & info = v_info_;
    auto height = info.total;

    auto view_port = this->getHeight();
    if (height <= view_port)
        height = view_port;
    
    info.min_pos = 0;
    info.max_pos = height - view_port;
    auto cur_pos = info.cur_pos;
    if (cur_pos > info.max_pos)
        cur_pos = info.max_pos;
    if (cur_pos < info.min_pos)
        cur_pos = info.min_pos;
    info.cur_pos = cur_pos;
    container_->setTop(-info.cur_pos);
    container_->setHeight(height);

    delegate_ ? delegate_->onScrollRange(
        this, info.min_pos, info.max_pos, view_port, false) : 0;
    delegate_ ? delegate_->onScrollPos(this, info.cur_pos, false) : 0;
}

void Scroller::updateHInfo()
{
    auto & info = h_info_;
    auto width = info.total;

    auto view_port = this->getWidth();
    if (width <= view_port)
        width = view_port;

    info.min_pos = 0;
    info.max_pos = width - view_port;
    auto cur_pos = info.cur_pos;
    if (cur_pos > info.max_pos)
        cur_pos = info.max_pos;
    if (cur_pos < info.min_pos)
        cur_pos = info.min_pos;
    info.cur_pos = cur_pos;
    container_->setLeft(-info.cur_pos);
    container_->setWidth(width);
    delegate_ ? delegate_->onScrollRange(
        this, info.min_pos, info.max_pos, view_port, true) : 0;
    delegate_ ? delegate_->onScrollPos(this, info.cur_pos, true) : 0;
}


void Scroller::onSizeChanged()
{
    updateVInfo();
    updateHInfo();
    Area::onSizeChanged();
}

void Scroller::onWheel(WheelEvent & e)
{//只处理冒泡阶段的
    Area::onWheel(e);
    if (e.canceled())
        return;
    if (Event::kBubbling != e.phase() )
        return;

    auto path = e.getPath();
    if (!path)
        return;
    for (auto iter = path->begin(); iter != path->end(); ++iter)
    {//为了防止scroller嵌套 如果滚动在scroller子孙中的一个scroller发生则不管
        if (this == (*iter).get())
            break;
        //scroller richedit webview本身就是可以滚动的 所以如果事件经过这些就不处理
        if (kClassScroller == (*iter)->getClassName() ||
            kClassRichEdit == (*iter)->getClassName() ||
            kClassWebView == (*iter)->getClassName())
        {
            return;
        }
    }
    if (e.dx() != 0)
        setScrollPos(h_info_.cur_pos - e.dx() / (WHEEL_DELTA / speed_), true);
    if (e.dy() != 0)
        setScrollPos(v_info_.cur_pos - e.dy() / (WHEEL_DELTA / speed_), false);
}

}