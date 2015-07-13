#include "area.h"

namespace bones
{


Area::Area()
{
    ;
}

void Area::bind(EventType type, Event::Phase phase, const CFEvent & cf)
{
    delegates_[phase][type] = cf;
}

void Area::bind(const CFNotifyOnSizeChanged & cf)
{
    on_size_changed_ = cf;
}

const char * Area::getClassName() const
{
    return kClassArea;
}

void Area::onMouseEnter(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onMouseMove(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onMouseDown(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onMouseUp(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onMouseClick(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onMouseDClick(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onMouseLeave(MouseEvent & e)
{
    pushEvent(e);
}

void Area::onKeyDown(KeyEvent & e)
{
    pushEvent(e);
}

void Area::onKeyUp(KeyEvent & e)
{
    pushEvent(e);
}

void Area::onFocusOut(FocusEvent & e)
{
    pushEvent(e);
}

void Area::onFocusIn(FocusEvent & e)
{
    pushEvent(e);
}

void Area::onBlur(FocusEvent & e)
{
    pushEvent(e);
}

void Area::onFocus(FocusEvent & e)
{
    pushEvent(e);
}

void Area::onWheel(WheelEvent & e)
{
    pushEvent(e);
}

void Area::onSizeChanged()
{
    on_size_changed_ ? on_size_changed_(*this): 0;
}

void Area::pushEvent(Event & e)
{
    delegates_[e.phase()][e.type()] ? delegates_[e.phase()][e.type()](*this, e) : 0;
}

}