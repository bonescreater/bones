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
static const char * kMethodIsCapsLockDown = "isCapsLockDown";
static const char * kMethodIsAltDown = "isAltDown";
static const char * kMethodIsAltGrDown = "isAltGrDown";
static const char * kMethodIsCommandDown = "isCommandDown";
static const char * kMethodIsLeftMouseDown = "isLeftMouseDown";
static const char * kMethodIsMiddleMouseDown = "isMiddleMouseDown";
static const char * kMethodIsRightMouseDown = "isRightMouseDown";

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

static void GetEvent(lua_State * l, const char * class_name)
{
    assert(l && class_name);
    luaL_getmetatable(l, class_name);
    if (!lua_istable(l, -1))
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
}

static int IsShiftDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isShiftDown());

    return 1;
}

static int IsControlDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isControlDown());

    return 1;
}

static int IsCapsLockDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isCapsLockDown());

    return 1;
}

static int IsAltDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isAltDown());

    return 1;
}

static int IsAltGrDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isAltGrDown());

    return 1;
}

static int IsCommandDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isCommandDown());

    return 1;
}

static int IsLeftMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeftMouseDown());

    return 1;
}

static int IsMiddleMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isMiddleMouseDown());

    return 1;
}

static int IsRightMouseDown(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesEventFlag **)lua_touserdata(l, 1);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRightMouseDown());

    return 1;
}

static void GetUIEvent(lua_State * l, const char * class_name)
{
    GetEvent(l, class_name);
    lua_pushcfunction(l, &IsShiftDown);
    lua_setfield(l, -2, kMethodIsShiftDown);
    lua_pushcfunction(l, &IsControlDown);
    lua_setfield(l, -2, kMethodIsControlDown);
    lua_pushcfunction(l, &IsCapsLockDown);
    lua_setfield(l, -2, kMethodIsCapsLockDown);
    lua_pushcfunction(l, &IsAltDown);
    lua_setfield(l, -2, kMethodIsAltDown);
    lua_pushcfunction(l, &IsAltGrDown);
    lua_setfield(l, -2, kMethodIsAltGrDown);
    lua_pushcfunction(l, &IsCommandDown);
    lua_setfield(l, -2, kMethodIsCommandDown);
    lua_pushcfunction(l, &IsLeftMouseDown);
    lua_setfield(l, -2, kMethodIsLeftMouseDown);
    lua_pushcfunction(l, &IsMiddleMouseDown);
    lua_setfield(l, -2, kMethodIsMiddleMouseDown);
    lua_pushcfunction(l, &IsRightMouseDown);
    lua_setfield(l, -2, kMethodIsRightMouseDown);
}

/*............................................................
mouse event
*/
static int Loc(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
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

static int RootLoc(lua_State *l)
{
    lua_settop(l, 1);
    auto e = *(BonesMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
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

static int IsLeftMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isLeftMouse());
    return 1;
}

static int IsMiddleMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isMiddleMouse());
    return 1;
}

static int IsRightMouse(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushboolean(l, e->isRightMouse());

    return 1;
}

static int MouseType(lua_State * l)
{
    lua_settop(l, 1);
    auto e = *(BonesMouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    lua_pushnil(l);
    if (e)
        lua_pushinteger(l, e->type());
    return 1;
}

void LuaMouseEvent::GetMetaTable(lua_State * l)
{
    GetUIEvent(l, kMetaTableMouseEvent);
    lua_pushcfunction(l, &Loc);
    lua_setfield(l, -2, kMethodLoc);
    lua_pushcfunction(l, &RootLoc);
    lua_setfield(l, -2, kMethodRootLoc);
    lua_pushcfunction(l, &IsLeftMouse);
    lua_setfield(l, -2, kMethodIsLeftMouse);
    lua_pushcfunction(l, &IsMiddleMouse);
    lua_setfield(l, -2, kMethodIsMiddleMouse);
    lua_pushcfunction(l, &IsRightMouse);
    lua_setfield(l, -2, kMethodIsRightMouse);
    lua_pushcfunction(l, &MouseType);
    lua_setfield(l, -2, kMethodType);
}

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

void LuaKeyEvent::GetMetaTable(lua_State * l)
{
    GetUIEvent(l, kMetaTableKeyEvent);
    lua_pushcfunction(l, &Ch);
    lua_setfield(l, -2, kMethodCH);
    lua_pushcfunction(l, &State);
    lua_setfield(l, -2, kMethodState);
    lua_pushcfunction(l, &KeyType);
    lua_setfield(l, -2, kMethodType);
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
    GetUIEvent(l, kMetaTableFocusEvent);
    lua_pushcfunction(l, &IsTabTraversal);
    lua_setfield(l, -2, kMethodIsTabTraversal);
    lua_pushcfunction(l, &FocusType);
    lua_setfield(l, -2, kMethodType);
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

void LuaWheelEvent::GetMetaTable(lua_State * l)
{
    GetEvent(l, kMetaTableWheelEvent);
    lua_pushcfunction(l, &DX);
    lua_setfield(l, -2, kMethodDX);
    lua_pushcfunction(l, &DY);
    lua_setfield(l, -2, kMethodDY);
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