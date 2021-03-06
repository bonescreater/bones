﻿#include "event.h"
#include "view.h"

namespace bones
{

Event::Event() 
:type_(kET_COUNT), phase_(kCapturing), path_(nullptr),
bubbles_(true), cancelable_(true), canceled_(false), propagation_(true)
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

void Event::attach(const EventDispatcher::Path * path)
{
    path_ = path;
}

const EventDispatcher::Path * Event::getPath() const
{
    return path_;
}

bool UIEvent::isShiftDown() const 
{ 
    return !!(flags_ & kEF_SHIFT_DOWN);
}

bool UIEvent::isControlDown() const
{ 
    return !!(flags_ & kEF_CONTROL_DOWN);
}

bool UIEvent::isCapsLockOn() const
{ 
    return !!(flags_ & kEF_CAPS_LOCK_ON);
}

bool UIEvent::isAltDown() const
{ 
    return !!(flags_ & kEF_ALT_DOWN);
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

bool UIEvent::isNumLockOn() const
{
    return !!(flags_ & kEF_NUM_LOCK_ON);
}

bool UIEvent::isKeyPad() const
{
    return !!(flags_ & kEF_IS_KEY_PAD);
}

bool UIEvent::isLeft() const
{
    return !!(flags_ && kEF_IS_LEFT);
}

bool UIEvent::isRight() const
{
    return !!(flags_ && kEF_IS_RIGHT);
}

int UIEvent::getFlags() const
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

const Point & MouseEvent::getLoc() const
{
    return location_;
}

const Point & MouseEvent::getRootLoc() const
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

KeyEvent::KeyEvent(EventType type, View * target, 
    KeyboardCode value, KeyState state, 
    bool system, int flags)
{
    type_ = type;
    target_.reset(target);
    key_code_ = (KeyboardCode)value;
    state_ = state;
    flags_ = flags;
    system_ = system;
}

KeyboardCode KeyEvent::key() const
{
    return key_code_;
}

wchar_t KeyEvent::ch() const
{
    return ch_;
}

KeyState KeyEvent::state() const
{
    return state_;
}

bool KeyEvent::system() const
{
    return system_;
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

CompositionEvent * CompositionEvent::From(Event & e)
{
    if (kET_COMPOSITION_START == e.type() || 
        kET_COMPOSITION_UPDATE == e.type() ||
        kET_COMPOSITION_END == e.type())
        return static_cast<CompositionEvent *>(&e);
    return nullptr;
}

CompositionEvent::CompositionEvent(EventType type, View * target,
    long index, unsigned int dbcs,
    const wchar_t * str,
    long cursor)
{
    type_ = type;
    target_.reset(target);
    index_ = index;
    dbcs_ = dbcs;
    str_ = str;
    cursor_ = cursor;
}

long CompositionEvent::index() const
{
    return index_;
}

unsigned int CompositionEvent::dbcs() const
{
    return dbcs_;
}

const wchar_t * CompositionEvent::str() const
{
    return str_;
}

long CompositionEvent::cursor() const
{
    return cursor_;
}

}