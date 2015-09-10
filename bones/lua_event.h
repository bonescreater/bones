#ifndef BONES_LUA_EVENT_H_
#define BONES_LUA_EVENT_H_

#include "bones_internal.h"
#include "script_parser.h"
#include "core/event.h"
#include "lua_context.h"

namespace bones
{

class Event;

template<class Base, class Object>
class LuaEvent : public Base
{
public:
    LuaEvent(Object & e)
        :event_(&e)
    {

    }

    void stopPropagation() override
    {
        event_->stopPropagation();
    }

    void preventDefault() override
    {
        event_->preventDefault();
    }

    bool canceled() override
    {
        return event_->canceled();
    }

    BonesObject * target() override
    {
        return GetCoreInstance()->getObject(event_->target());
    }

    BonesEvent::Phase phase() override
    {
        auto phase = event_->phase();
        if (Event::kCapture == phase)
            return BonesEvent::kCapture;
        if (Event::kTarget == phase)
            return BonesEvent::kTarget;
        if (Event::kBubbling == phase)
            return BonesEvent::kBubbling;
        return BonesEvent::kPhaseCount;
    }
protected:
    Object * event_;
};

class LuaMouseEvent : public LuaEvent<BonesMouseEvent, MouseEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaMouseEvent(MouseEvent & e);

    Type type() const override;

    bool isLeftMouse() const override;

    bool isMiddleMouse() const override;

    bool isRightMouse() const override;

    BonesPoint getLoc() const override;

    BonesPoint getRootLoc() const override;

    bool isShiftDown() const override;

    bool isControlDown() const override;

    bool isAltDown() const override;

    bool isLeftMouseDown() const override;

    bool isMiddleMouseDown() const override;

    bool isRightMouseDown() const override;

    bool isCapsLockOn() const override;

    bool isNumLockOn() const override;
};

class LuaWheelEvent : public LuaEvent<BonesWheelEvent, WheelEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaWheelEvent(WheelEvent & e);

    int dx() const override;

    int dy() const override;

    bool isLeftMouse() const override;

    bool isMiddleMouse() const override;

    bool isRightMouse() const override;

    BonesPoint getLoc() const override;

    BonesPoint getRootLoc() const override;

    bool isShiftDown() const override;

    bool isControlDown() const override;

    bool isAltDown() const override;

    bool isLeftMouseDown() const override;

    bool isMiddleMouseDown() const override;

    bool isRightMouseDown() const override;

    bool isCapsLockOn() const override;

    bool isNumLockOn() const override;
};

class LuaKeyEvent : public LuaEvent<BonesKeyEvent, KeyEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaKeyEvent(KeyEvent & e);

    Type type() const override;

    wchar_t ch() const override;

    KeyState state() const override;

    bool system() const override;

    bool isShiftDown() const override;

    bool isControlDown() const override;

    bool isAltDown() const override;

    bool isCapsLockOn() const override;

    bool isNumLockOn() const override;

    bool isKeyPad() const override;

    bool isLeft() const override;

    bool isRight() const override;
};

class LuaFocusEvent : public LuaEvent<BonesFocusEvent, FocusEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaFocusEvent(FocusEvent & e);

    Type type() const override;

    bool isTabTraversal() const override;
};



}
#endif