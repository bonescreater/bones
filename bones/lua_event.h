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

template<class Base, class Object>
class LuaUIEvent : public LuaEvent<Base, Object>
{
public:
    LuaUIEvent(Object & e)
        :LuaEvent(e)
    {

    }
    bool isShiftDown() const override
    {
        return event_->isShiftDown();
    }

    bool isControlDown() const override
    {
        return event_->isControlDown();
    }

    bool isCapsLockDown() const override
    {
        return event_->isCapsLockDown();
    }

    bool isAltDown() const override
    {
        return event_->isAltDown();
    }

    bool isAltGrDown() const override
    {
        return event_->isAltGrDown();
    }

    bool isCommandDown() const override
    {
        return event_->isCommandDown();
    }

    bool isLeftMouseDown() const override
    {
        return event_->isLeftMouseDown();
    }

    bool isMiddleMouseDown() const override
    {
        return event_->isMiddleMouseDown();
    }

    bool isRightMouseDown() const override
    {
        return event_->isRightMouseDown();
    }
};

class LuaMouseEvent : public LuaUIEvent<BonesMouseEvent, MouseEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaMouseEvent(MouseEvent & e);

    virtual Type type() const override;

    virtual bool isLeftMouse() const override;

    virtual bool isMiddleMouse() const override;

    virtual bool isRightMouse() const override;

    virtual BonesPoint getLoc() const override;

    virtual BonesPoint getRootLoc() const override;

};

class LuaKeyEvent : public LuaUIEvent<BonesKeyEvent, KeyEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaKeyEvent(KeyEvent & e);

    Type type() const override;

    wchar_t ch() const override;

    KeyState state() const override;
};

class LuaFocusEvent : public LuaEvent<BonesFocusEvent, FocusEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaFocusEvent(FocusEvent & e);

    Type type() const override;

    bool isTabTraversal() const override;
};

class LuaWheelEvent : public LuaUIEvent<BonesWheelEvent, WheelEvent>
{
public:
    static void GetMetaTable(lua_State * l);

    LuaWheelEvent(WheelEvent & e);

    int dx() const override;

    int dy() const override;
};

}
#endif