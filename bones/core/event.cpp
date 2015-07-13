#include "event.h"
#include "view.h"

namespace bones
{

Event::Event() 
:type_(kET_COUNT), phase_(kCapture),
bubbles_(true), cancelable_(true), canceled_(false), propagation_(true), user_data_(nullptr)
{

}

void Event::stopPropagation()
{
    propagation_ = false;
}

void Event::preventDefault()
{
    if (cancelable_)
        canceled_ = true;
}

bool Event::canceled() const
{
    if (cancelable_)
        return canceled_;

    return false;
}

View * Event::target() const
{
    return target_.get();
}

EventType Event::type() const
{
    return type_;
}

Event::Phase Event::phase() const
{
    return phase_;
}

void Event::setUserData(void * user_data)
{
    user_data_ = user_data;
}

bool UIEvent::isShiftDown() const 
{ 
    return !!(flags_ & kEF_SHIFT_DOWN);
}

bool UIEvent::isControlDown() const
{ 
    return !!(flags_ & kEF_CONTROL_DOWN);
}

bool UIEvent::isCapsLockDown() const
{ 
    return !!(flags_ & kEF_CAPS_LOCK_DOWN);
}

bool UIEvent::isAltDown() const
{ 
    return !!(flags_ & kEF_ALT_DOWN);
}

bool UIEvent::isAltGrDown() const
{ 
    return !!(flags_ & kEF_ALTGR_DOWN);
}

bool UIEvent::isCommandDown() const
{
    return !!(flags_ & kEF_COMMAND_DOWN);
}

bool UIEvent::isLeftMouseDown() const
{
    return !!(flags_ & kEF_LEFT_MOUSE_DOWN);
}

bool UIEvent::isMiddleMouseDown() const
{
    return !!(flags_ & kEF_MIDDLE_MOUSE_DOWN);
}

bool UIEvent::isRightMouseDown() const
{
    return !!(flags_ & kEF_RIGHT_MOUSE_DOWN);
}

//bool UIEvent::isOnlyLeftMouseDown() const
//{
//    return (flags() & kEF_LEFT_MOUSE_DOWN) &&
//        !(flags() & (kEF_MIDDLE_MOUSE_DOWN | kEF_RIGHT_MOUSE_DOWN));
//}
//
//bool UIEvent::isOnlyMiddleMouseDown() const
//{
//    return (flags() & kEF_MIDDLE_MOUSE_DOWN) &&
//        !(flags() & (kEF_LEFT_MOUSE_DOWN | kEF_RIGHT_MOUSE_DOWN));
//}
//
//bool UIEvent::isOnlyRightMouseDown() const
//{
//    return (flags() & kEF_RIGHT_MOUSE_DOWN) &&
//        !(flags() & (kEF_LEFT_MOUSE_DOWN | kEF_MIDDLE_MOUSE_DOWN));
//}

int UIEvent::flags() const
{
    return flags_;
}

MouseEvent * MouseEvent::From(Event & e)
{
    if (e.type() >= kET_MOUSE_ENTER && e.type() <= kET_MOUSE_LEAVE)
        return static_cast<MouseEvent *>(&e);
    return nullptr;
}

MouseEvent::MouseEvent(EventType type, MouseButton button, View * target,
    const Point & location,
    const Point & root_location,
    int flags)
{
    assert(type <= kET_MOUSE_LEAVE || type == kET_MOUSE_WHEEL);

    type_ = type;
    button_ = button;
    target_.reset(target);
    location_ = location;
    root_location_ = root_location;
    flags_ = flags;
}

bool MouseEvent::isLeftMouse() const
{
    return kMB_LEFT == button_;
}

bool MouseEvent::isMiddleMouse() const
{
    return kMB_MIDDLE == button_;
}

bool MouseEvent::isRightMouse() const
{
    return kMB_RIGHT == button_;
}

const Point & MouseEvent::loc() const
{
    return location_;
}

const Point & MouseEvent::rootLoc() const
{
    return root_location_;
}

MouseButton MouseEvent::button() const
{
    return button_;
}

KeyEvent * KeyEvent::From(Event & e)
{
    if (e.type() >= kET_KEY_DOWN && e.type() <= kET_KEY_UP)
        return static_cast<KeyEvent *>(&e);
    return nullptr;
}

KeyEvent::KeyEvent(EventType type, View * target, KeyboardCode code, int flags)
{
    type_ = type;
    target_.reset(target);
    key_code_ = code;
    flags_ = flags;
}

KeyboardCode KeyEvent::key() const
{
    return key_code_;
}

FocusEvent * FocusEvent::From(Event & e)
{
    if (e.type() >= kET_FOCUS_OUT && e.type() <= kET_FOCUS)
        return static_cast<FocusEvent *>(&e);
    return nullptr;
}

FocusEvent::FocusEvent(EventType type, View * target, bool tab)
    :tab_traversal_(false)
{
    type_ = type;
    target_.reset(target);
    cancelable_ = false;
    tab_traversal_ = tab;
    if (kET_FOCUS == type_ || kET_BLUR == type_)
        bubbles_ = false;
}

bool FocusEvent::isTabTraversal() const
{
    return tab_traversal_;
}

WheelEvent * WheelEvent::From(Event & e)
{
    if (kET_MOUSE_WHEEL == e.type())
        return static_cast<WheelEvent *>(&e);
    return nullptr;
}

WheelEvent::WheelEvent(EventType type, View * target, int dx, int dy, 
                       const Point & location,
                       const Point & root_location,
                       int flags)
:MouseEvent(type, kMB_NONE, target, location, root_location, flags)
{
    dx_ = dx;
    dy_ = dy;
}

int WheelEvent::dx() const
{
    return dx_;
}

int WheelEvent::dy() const
{
    return dy_;
}


}