#include "lua_event.h"
#include "bones_internal.h"
#include "script_parser.h"

namespace bones
{

static const char * kMetaTableMouseEvent = "__mt_mouse_event";
static const char * kMetaTableFocusEvent = "__mt_focus_event";
static const char * kMetaTableKeyEvent = "__mt_key_event";
static const char * kMetaTableWheelEvent = "__mt_wheel_event";

static const char * kMethodStopPropagation = "stopPropagation";
static const char * kMethodPreventDefault = "preventDefault";
static const char * kMethodCanceled = "canceled";
static const char * kMethodTarget = "target";
static const char * kMethodType = "type";
static const char * kMethodPhase = "phase";

static const char * kMethodIsShiftDown = "isShiftDown";
static const char * kMethodIsControlDown = "isControlDown";
static const char * kMethodIsAltDown = "isAltDown";
static const char * kMethodIsLeftMouseDown = "isLeftMouseDown";
static const char * kMethodIsMiddleMouseDown = "isMiddleMouseDown";
static const char * kMethodIsRightMouseDown = "isRightMouseDown";
static const char * kMethodIsCapsLockOn = "isCapsLockOn";
static const char * kMethodIsNumLockOn = "isNumLockOn";
static const char * kMethodIsKeyPad = "isKeyPad";
static const char * kMethodIsLeft = "isLeft";
static const char * kMethodIsRight = "isRight";

static const char * kMethodLoc = "getLoc";
static const char * kMethodRootLoc = "getRootLoc";
static const char * kMethodIsLeftMouse = "isLeftMouse";
static const char * kMethodIsMiddleMouse = "isMiddleMouse";
static const char * kMethodIsRightMouse = "isRightMouse";

static const char * kMethodCH = "ch";
static const char * kMethodState = "state";

static const char * kMethodIsTabTraversal = "isTabTraversal";

static const char * kMethodDX = "dx";
static const char * kMethodDY = "dy";

/*
event
*/
static int StopPropagation(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEvent **)lua_touserdata(l, 1);
    if (e)
        e->stopPropagation();
    return 0;
}

static int PreventDefault(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEvent **)lua_touserdata(l, 1);
    if (e)
        e->preventDefault();
    return 0;
}

static int Canceled(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEvent **)lua_touserdata(l, 1);
    if (e)
        e->canceled();
    return 0;
}

static int Target(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    BonesObject * target = nullptr;
    if (e)
        target = e->target();
    if (target)
        LuaContext::GetLOFromCO(l, target);

    return 1;
}

static int Phase(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);

    if (e)
        lua_pushinteger(l, e->phase());
    return 1;
}

static bool GetEvent(lua_State * l, const char * class_name)
{
    assert(l && class_name);
    luaL_getmetatable(l, class_name);
    bool exist = lua_istable(l, -1);
    if (!exist)
    {
        lua_pop(l, 1);
        luaL_newmetatable(l, class_name);
        lua_pushnil(l);
        lua_copy(l, -2, -1);
        lua_setfield(l, -2, kMethodIndex);

        lua_pushcfunction(l, &StopPropagation);
        lua_setfield(l, -2, kMethodStopPropagation);
        lua_pushcfunction(l, &PreventDefault);
        lua_setfield(l, -2, kMethodPreventDefault);
        lua_pushcfunction(l, &Canceled);
        lua_setfield(l, -2, kMethodCanceled);
        lua_pushcfunction(l, &Target);
        lua_setfield(l, -2, kMethodTarget);
        lua_pushcfunction(l, &Phase);
        lua_setfield(l, -2, kMethodPhase);
    }
    return exist;
}

//static int IsMouseShiftDown(lua_State * l)
//{
//    lua_settop(l, 1);
//    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
//    lua_pushnil(l);
//    if (e)
//        lua_pushboolean(l, e->isShiftDown());
//
//    return 1;
//}
//
//static int IsControlDown(lua_State * l)
//{
//    lua_settop(l, 1);
//    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
//    lua_pushnil(l);
//    if (e)
//        lua_pushboolean(l, e->isControlDown());
//
//    return 1;
//}

//static void GetUIEvent(lua_State * l, const char * class_name)
//{
//    GetEvent(l, class_name);
//    lua_pushcfunction(l, &IsShiftDown);
//    lua_setfield(l, -2, kMethodIsShiftDown);
//    lua_pushcfunction(l, &IsControlDown);
//    lua_setfield(l, -2, kMethodIsControlDown);
//    lua_pushcfunction(l, &IsCapsLockOn);
//    lua_setfield(l, -2, kMethodIsCapsLockOn);
//    lua_pushcfunction(l, &IsAltDown);
//    lua_setfield(l, -2, kMethodIsAltDown);
//    lua_pushcfunction(l, &IsAltGrDown);
//    lua_setfield(l, -2, kMethodIsAltGrDown);
//    lua_pushcfunction(l, &IsCommandDown);
//    lua_setfield(l, -2, kMethodIsCommandDown);
//    lua_pushcfunction(l, &IsLeftMouseDown);
//    lua_setfield(l, -2, kMethodIsLeftMouseDown);
//    lua_pushcfunction(l, &IsMiddleMouseDown);
//    lua_setfield(l, -2, kMethodIsMiddleMouseDown);
//    lua_pushcfunction(l, &IsRightMouseDown);
//    lua_setfield(l, -2, kMethodIsRightMouseDown);
//}

/*............................................................
mouse event
*/
static int MouseLoc(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    lua_pushnil(l);
    if (e)
    {
        auto bp = e->getLoc();
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
    }
    return 2;
}
    
static int MouseRootLoc(lua_State *l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    lua_pushnil(l);
    if (e)
    {
        auto bp = e->getRootLoc();
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
    }
    return 2;
}
    
static int MouseIsLeftMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeftMouse());
    return 1;
}

static int MouseIsMiddleMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isMiddleMouse());
    return 1;
}

static int MouseIsRightMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRightMouse());
        
    return 1;
}

static int MouseType(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent); 
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->type());
    return 1;
}

static int MouseIsShiftDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isShiftDown());

    return 1;
}

static int MouseIsControlDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isControlDown());

    return 1;
}

static int MouseIsAltDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isAltDown());

    return 1;
}

static int MouseIsLeftMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeftMouseDown());

    return 1;
}

static int MouseIsMiddleMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isMiddleMouseDown());

    return 1;
}

static int MouseIsRightMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRightMouseDown());

    return 1;
}

static int MouseIsCapsLockOn(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isCapsLockOn());

    return 1;
}

static int MouseIsNumLockOn(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaMouseEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isNumLockOn());

    return 1;
}


void LuaMouseEvent::GetMetaTable(lua_State * l)
{
    if (!GetEvent(l, kMetaTableMouseEvent))
    {
        lua_pushcfunction(l, &MouseLoc);
        lua_setfield(l, -2, kMethodLoc);
        lua_pushcfunction(l, &MouseRootLoc);
        lua_setfield(l, -2, kMethodRootLoc);
        lua_pushcfunction(l, &MouseIsLeftMouse);
        lua_setfield(l, -2, kMethodIsLeftMouse);
        lua_pushcfunction(l, &MouseIsMiddleMouse);
        lua_setfield(l, -2, kMethodIsMiddleMouse);
        lua_pushcfunction(l, &MouseIsRightMouse);
        lua_setfield(l, -2, kMethodIsRightMouse);
        lua_pushcfunction(l, &MouseType);
        lua_setfield(l, -2, kMethodType);

        lua_pushcfunction(l, &MouseIsShiftDown);
        lua_setfield(l, -2, kMethodIsShiftDown);
        lua_pushcfunction(l, &MouseIsControlDown);
        lua_setfield(l, -2, kMethodIsControlDown);
        lua_pushcfunction(l, &MouseIsAltDown);
        lua_setfield(l, -2, kMethodIsAltDown);
        lua_pushcfunction(l, &MouseIsLeftMouseDown);
        lua_setfield(l, -2, kMethodIsLeftMouseDown);
        lua_pushcfunction(l, &MouseIsMiddleMouseDown);
        lua_setfield(l, -2, kMethodIsMiddleMouseDown);
        lua_pushcfunction(l, &MouseIsRightMouseDown);
        lua_setfield(l, -2, kMethodIsRightMouseDown);
        lua_pushcfunction(l, &MouseIsCapsLockOn);
        lua_setfield(l, -2, kMethodIsCapsLockOn);
        lua_pushcfunction(l, &MouseIsNumLockOn);
        lua_setfield(l, -2, kMethodIsNumLockOn);
    }
}

LuaMouseEvent::LuaMouseEvent(MouseEvent & e)
:LuaEvent(e)
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

/*............................................................
WHEEL event
*/
static int DX(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesWheelEvent **)luaL_checkudata(l, 1, kMetaTableWheelEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->dx());
    return 1;
}

static int DY(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesWheelEvent **)luaL_checkudata(l, 1, kMetaTableWheelEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->dy());
    return 1;
}

static int WheelLoc(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    lua_pushnil(l);
    if (e)
    {
        auto bp = e->getLoc();
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
    }
    return 2;
}

static int WheelRootLoc(lua_State *l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    lua_pushnil(l);
    if (e)
    {
        auto bp = e->getRootLoc();
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
    }
    return 2;
}

static int WheelIsLeftMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeftMouse());
    return 1;
}

static int WheelIsMiddleMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isMiddleMouse());
    return 1;
}

static int WheelIsRightMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRightMouse());

    return 1;
}

static int WheelIsShiftDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isShiftDown());

    return 1;
}

static int WheelIsControlDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isControlDown());

    return 1;
}

static int WheelIsAltDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isAltDown());

    return 1;
}

static int WheelIsLeftMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeftMouseDown());

    return 1;
}

static int WheelIsMiddleMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isMiddleMouseDown());

    return 1;
}

static int WheelIsRightMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRightMouseDown());

    return 1;
}

static int WheelIsCapsLockOn(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isCapsLockOn());

    return 1;
}

static int WheelIsNumLockOn(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaWheelEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isNumLockOn());

    return 1;
}

void LuaWheelEvent::GetMetaTable(lua_State * l)
{
    GetEvent(l, kMetaTableWheelEvent);
    lua_pushcfunction(l, &DX);
    lua_setfield(l, -2, kMethodDX);
    lua_pushcfunction(l, &DY);
    lua_setfield(l, -2, kMethodDY);

    lua_pushcfunction(l, &WheelLoc);
    lua_setfield(l, -2, kMethodLoc);
    lua_pushcfunction(l, &WheelRootLoc);
    lua_setfield(l, -2, kMethodRootLoc);
    lua_pushcfunction(l, &WheelIsLeftMouse);
    lua_setfield(l, -2, kMethodIsLeftMouse);
    lua_pushcfunction(l, &WheelIsMiddleMouse);
    lua_setfield(l, -2, kMethodIsMiddleMouse);
    lua_pushcfunction(l, &WheelIsRightMouse);
    lua_setfield(l, -2, kMethodIsRightMouse);

    lua_pushcfunction(l, &WheelIsShiftDown);
    lua_setfield(l, -2, kMethodIsShiftDown);
    lua_pushcfunction(l, &WheelIsControlDown);
    lua_setfield(l, -2, kMethodIsControlDown);
    lua_pushcfunction(l, &WheelIsAltDown);
    lua_setfield(l, -2, kMethodIsAltDown);
    lua_pushcfunction(l, &WheelIsLeftMouseDown);
    lua_setfield(l, -2, kMethodIsLeftMouseDown);
    lua_pushcfunction(l, &WheelIsMiddleMouseDown);
    lua_setfield(l, -2, kMethodIsMiddleMouseDown);
    lua_pushcfunction(l, &WheelIsRightMouseDown);
    lua_setfield(l, -2, kMethodIsRightMouseDown);
    lua_pushcfunction(l, &WheelIsCapsLockOn);
    lua_setfield(l, -2, kMethodIsCapsLockOn);
    lua_pushcfunction(l, &WheelIsNumLockOn);
    lua_setfield(l, -2, kMethodIsNumLockOn);
}

LuaWheelEvent::LuaWheelEvent(WheelEvent & e)
:LuaEvent(e)
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

#define LUA_MOUSE_DEFINE(T)\
    bool T##::isLeftMouse() const\
{\
    return event_->isLeftMouse(); \
}\
    \
    bool T##::isMiddleMouse() const\
{\
    return event_->isMiddleMouse(); \
}\
    \
    bool T##::isRightMouse() const\
{\
    return event_->isRightMouse(); \
}\
    \
    BonesPoint T##::getLoc() const\
{\
    auto & p = event_->getLoc(); \
    BonesPoint bp = { p.x(), p.y() }; \
    return bp; \
}\
    \
    BonesPoint T##::getRootLoc() const\
{\
    auto & p = event_->getRootLoc(); \
    BonesPoint bp = { p.x(), p.y() }; \
    return bp; \
}\
    \
    bool T##::isShiftDown() const\
{\
    return event_->isShiftDown(); \
}\
    \
    bool T##::isControlDown() const\
{\
    return event_->isControlDown(); \
}\
    \
    bool T##::isAltDown() const\
{\
    return event_->isAltDown(); \
}\
    \
    bool T##::isLeftMouseDown() const\
{\
    return event_->isLeftMouseDown(); \
}\
    \
    bool T##::isMiddleMouseDown() const\
{\
    return event_->isMiddleMouseDown(); \
}\
    \
    bool T##::isRightMouseDown() const\
{\
    return event_->isRightMouseDown(); \
}\
    \
    bool T##::isCapsLockOn() const\
{\
    return event_->isCapsLockOn(); \
}\
    \
    bool T##::isNumLockOn() const\
{\
    return event_->isNumLockOn(); \
}\

LUA_MOUSE_DEFINE(LuaMouseEvent)
LUA_MOUSE_DEFINE(LuaWheelEvent)


/*............................................................
key event
*/
static int Ch(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesKeyEvent **)luaL_checkudata(l, 1, kMetaTableKeyEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->ch());
    return 1;
}

static int State(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesKeyEvent **)luaL_checkudata(l, 1, kMetaTableKeyEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->state().state);
    return 1;
}

static int KeyType(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesKeyEvent **)luaL_checkudata(l, 1, kMetaTableKeyEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->type());
    return 1;
}

static int KeyIsShiftDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isShiftDown());
    
    return 1;
}

static int KeyIsControlDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isControlDown());

    return 1;
}

static int KeyIsAltDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isAltDown());

    return 1;
}

static int KeyIsCapsLockOn(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isCapsLockOn());

    return 1;
}

static int KeyIsNumLockOn(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isNumLockOn());

    return 1;
}

static int KeyIsKeyPad(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isKeyPad());

    return 1;
}

static int KeyIsLeft(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeft());

    return 1;
}

static int KeyIsRight(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(LuaKeyEvent **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRight());

    return 1;
}

void LuaKeyEvent::GetMetaTable(lua_State * l)
{
    if (!GetEvent(l, kMetaTableKeyEvent))
    {
        lua_pushcfunction(l, &Ch);
        lua_setfield(l, -2, kMethodCH);
        lua_pushcfunction(l, &State);
        lua_setfield(l, -2, kMethodState);
        lua_pushcfunction(l, &KeyType);
        lua_setfield(l, -2, kMethodType);

        lua_pushcfunction(l, &KeyIsShiftDown);
        lua_setfield(l, -2, kMethodIsShiftDown);
        lua_pushcfunction(l, &KeyIsControlDown);
        lua_setfield(l, -2, kMethodIsControlDown);
        lua_pushcfunction(l, &KeyIsAltDown);
        lua_setfield(l, -2, kMethodIsAltDown);
        lua_pushcfunction(l, &KeyIsCapsLockOn);
        lua_setfield(l, -2, kMethodIsCapsLockOn);
        lua_pushcfunction(l, &KeyIsNumLockOn);
        lua_setfield(l, -2, kMethodIsNumLockOn);
        lua_pushcfunction(l, &KeyIsKeyPad);
        lua_setfield(l, -2, kMethodIsKeyPad);
        lua_pushcfunction(l, &KeyIsLeft);
        lua_setfield(l, -2, kMethodIsLeft);
        lua_pushcfunction(l, &KeyIsRight);
        lua_setfield(l, -2, kMethodIsRight);
    }
}

LuaKeyEvent::LuaKeyEvent(KeyEvent & e)
:LuaEvent(e)
{

}

LuaKeyEvent::Type LuaKeyEvent::type() const
{
    auto type = event_->type();

    if (kET_KEY_DOWN == type)
        return kKeyDown;
    if (kET_CHAR == type)
        return kChar;
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

bool LuaKeyEvent::system() const
{
    return event_->system();
}

bool LuaKeyEvent::isShiftDown() const
{
    return event_->isShiftDown();
}

bool LuaKeyEvent::isControlDown() const
{
    return event_->isControlDown();
}

bool LuaKeyEvent::isAltDown() const
{
    return event_->isAltDown();
}

bool LuaKeyEvent::isCapsLockOn() const
{
    return event_->isCapsLockOn();
}

bool LuaKeyEvent::isNumLockOn() const
{
    return event_->isNumLockOn();
}

bool LuaKeyEvent::isKeyPad() const
{
    return event_->isKeyPad();
}

bool LuaKeyEvent::isLeft() const
{
    return event_->isLeft();
}

bool LuaKeyEvent::isRight() const
{
    return event_->isRight();
}
/*............................................................
focus event
*/
static int IsTabTraversal(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesFocusEvent **)luaL_checkudata(l, 1, kMetaTableFocusEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isTabTraversal());
    return 1;
}

static int FocusType(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesFocusEvent **)luaL_checkudata(l, 1, kMetaTableFocusEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->type());
    return 1;
}

void LuaFocusEvent::GetMetaTable(lua_State * l)
{
    if (!GetEvent(l, kMetaTableFocusEvent))
    {
        lua_pushcfunction(l, &IsTabTraversal);
        lua_setfield(l, -2, kMethodIsTabTraversal);
        lua_pushcfunction(l, &FocusType);
        lua_setfield(l, -2, kMethodType);
    }
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



}