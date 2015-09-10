#include "area.h"

namespace bones
{


Area::Area()
:delegate_(nullptr)
{
    ;
}

void Area::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

const char * Area::getClassName() const
{
    return kClassArea;
}

void Area::onMouseEnter(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseEnter(this, e) : 0;
}

void Area::onMouseMove(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseMove(this, e) : 0;
}

void Area::onMouseDown(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseDown(this, e) : 0;
}

void Area::onMouseUp(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseUp(this, e) : 0;
}

void Area::onMouseClick(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseClick(this, e) : 0;
}

void Area::onMouseDClick(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseDClick(this, e) : 0;
}

void Area::onMouseLeave(MouseEvent & e)
{
    delegate_ ? delegate_->onMouseLeave(this, e) : 0;
}

void Area::onKeyDown(KeyEvent & e)
{
    delegate_ ? delegate_->onKeyDown(this, e) : 0;
}

void Area::onKeyUp(KeyEvent & e)
{
    delegate_ ? delegate_->onKeyUp(this, e) : 0;
}

void Area::onChar(KeyEvent & e)
{
    delegate_ ? delegate_->onChar(this, e) : 0;
}

void Area::onFocusOut(FocusEvent & e)
{
    delegate_ ? delegate_->onFocusOut(this, e) : 0;
}

void Area::onFocusIn(FocusEvent & e)
{
    delegate_ ? delegate_->onFocusIn(this, e) : 0;

}

void Area::onBlur(FocusEvent & e)
{
    delegate_ ? delegate_->onBlur(this, e) : 0;
}

void Area::onFocus(FocusEvent & e)
{
    delegate_ ? delegate_->onFocus(this, e) : 0;
}

void Area::onWheel(WheelEvent & e)
{
    delegate_ ? delegate_->onWheel(this, e) : 0;
}

void Area::onSizeChanged()
{
    delegate_ ? delegate_->onSizeChanged(this, getSize()) : 0;
}

void Area::onPositionChanged()
{
    delegate_ ? delegate_->onPositionChanged(this, getLoc()) : 0;
}

bool Area::onHitTest(const Point & loc)
{
    return delegate_ ? delegate_->onHitTest(this, loc) : true;
}


}