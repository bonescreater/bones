#include "lua_event.h"
#include "bones_internal.h"
#include "script_parser.h"

namespace bones
{

//void LuaEvent::Get(lua_State * l, Event & e)
//{
//    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
//
//    LuaContext::GetEventCache(l);
//    void **userdata = (void **)lua_touserdata(l, -1);
//    if (userdata == nullptr)
//        return;
//
//    *userdata = &e;
//    if (MouseEvent::From(e))
//        LuaMetaTable::GetMouseEvent(l);
//    else if (FocusEvent::From(e))
//        LuaMetaTable::GetFocusEvent(l);
//    else
//    {
//        lua_pushnil(l);
//        LOG_VERBOSE << "unsupport event metatable\n";
//    }
//        
//
//    lua_setmetatable(l, -2);
//}

LuaMouseEvent::LuaMouseEvent(MouseEvent & e)
:LuaUIEvent(e)
{

}

LuaMouseEvent::Type LuaMouseEvent::type() const
{
    auto type = event_->type();
    if (kET_MOUSE_ENTER == type)
        return kMouseEnter;
    if (kET_MOUSE_MOVE == type)
        return kMouseMove;
    if (kET_MOUSE_DOWN == type)
        return kMouseDown;
    if (kET_MOUSE_UP == type)
        return kMouseUp;
    if (kET_MOUSE_CLICK == type)
        return kMouseClick;
    if (kET_MOUSE_DCLICK == type)
        return kMouseDClick;
    if (kET_MOUSE_LEAVE == type)
        return kMouseLeave;
    return kTypeCount;
}

bool LuaMouseEvent::isLeftMouse() const
{
    return event_->isLeftMouse();
}

bool LuaMouseEvent::isMiddleMouse() const
{
    return event_->isMiddleMouse();
}

bool LuaMouseEvent::isRightMouse() const
{
    return event_->isRightMouse();
}

BonesPoint LuaMouseEvent::getLoc() const
{
    auto & loc = event_->getLoc();

    BonesPoint bp = { loc.x(), loc.y() };
    return bp;
}

BonesPoint LuaMouseEvent::getRootLoc() const
{
    auto & loc = event_->getRootLoc();

    BonesPoint bp = { loc.x(), loc.y() };
    return bp;
}

LuaKeyEvent::LuaKeyEvent(KeyEvent & e)
:LuaUIEvent(e)
{

}

LuaKeyEvent::Type LuaKeyEvent::type() const
{
    auto type = event_->type();

    if (kET_KEY_DOWN == type)
        return kKeyDown;
    if (kET_KEY_PRESS == type)
        return kKeyPress;
    if (kET_KEY_UP == type)
        return kKeyUp;

    return kTypeCount;
}

wchar_t LuaKeyEvent::ch() const
{
    return event_->ch();
}

LuaKeyEvent::KeyState LuaKeyEvent::state() const
{
    return *(KeyState *)(&event_->state());
}

LuaFocusEvent::LuaFocusEvent(FocusEvent & e)
:LuaEvent(e)
{

}

LuaFocusEvent::Type LuaFocusEvent::type() const
{
    auto type = event_->type();
    if (kET_FOCUS_IN == type)
        return kFocusIn;
    if (kET_FOCUS_OUT == type)
        return kFocusOut;
    if (kET_BLUR == type)
        return kBlur;
    if (kET_FOCUS == type)
        return kFocus;

    return kTypeCount;
}


bool LuaFocusEvent::isTabTraversal() const
{
    return event_->isTabTraversal();
}

LuaWheelEvent::LuaWheelEvent(WheelEvent & e)
:LuaUIEvent(e)
{

}

int LuaWheelEvent::dx() const 
{
    return event_->dx();
}

int LuaWheelEvent::dy() const
{
    return event_->dy();
}


}