#include "lua_meta_table.h"
#include "lua_context.h"
#include "lua_check.h"

#include "core/root.h"
#include "core/view.h"
#include "core/logging.h"
#include "core/text.h"
#include "core/encoding.h"
#include "core/animation_manager.h"
#include "core/animation.h"
#include "core/rich_edit.h"
#include "lua_animation.h"
#include "script_parser.h"

namespace bones
{
static const char * kMetaTablePanel = "__bone__panel__";

static const char * kMetaTableMouseEvent = "__mt_mouse_event";
static const char * kMetaTableFocusEvent = "__mt_focus_event";
static const char * kMetaTableAnimation = "__mt_animation";

static const char * kMethodIndex = "__index";
static const char * kMethodGC = "__gc";
static const char * kMethodGetOpacity = "getOpacity";
static const char * kMethodGetCObject = "getCObject";
static const char * kMethodAnimate = "animate";
static const char * kMethodStop = "stop";
static const char * kMethodPause = "pause";
static const char * kMethodResume = "resume";
static const char * kMethodStopAll = "stopAll";

static const char * kMethodContains = "contains";
static const char * kMethodApplyCSS = "applyCSS";
static const char * kMethodApplyClass = "applyClass";
static const char * kMethodGetChildAt = "getChildAt";
static const char * kMethodGetSize = "getSize";
static const char * kMethodGetLoc = "getLoc";

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

static const char * kMethodIsTabTraversal = "isTabTraversal";

//(self)
static int GC(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        BonesObject * ref = LuaMetaTable::CallGetBonesObject(l);
        if (ref)
            ref->release();
    }
    return 0;
}

static int GetOpacity(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
    if (bob)
        lua_pushnumber(l, bob->getOpacity());

    return 1;
}

//(self)
static int GetLoc(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushnil(l);

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
    if (bob)
    {
        BonesPoint bp = bob->getLoc();
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
    }
    return 2;
}


static int GetSize(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushnil(l);

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
    if (bob)
    {
        BonesSize bs = bob->getSize();
        lua_pushnumber(l, bs.width);
        lua_pushnumber(l, bs.height);
    }
    return 2;
}

static int Contains(lua_State * l)
{
    int count = lua_gettop(l);
    lua_pushnil(l);
    if (count == 3)
    {
        Scalar x = static_cast<Scalar>(lua_tonumber(l, 2));
        Scalar y = static_cast<Scalar>(lua_tonumber(l, 3));
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
        if (bob)
            lua_pushboolean(l, bob->contains(x, y));
    }
    return 1;
}

//(self, index)
static int GetChildAt(lua_State * l)
{
    int count = lua_gettop(l);
    lua_pushnil(l);
    if (count == 2)
    {
        size_t index = static_cast<size_t>(lua_tonumber(l, 2));
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
        if (bob)
        {
            auto child = bob->getChildAt(index);
            if (child)
                LuaContext::GetLOFromCO(l, child);
        }
          
    }
    return 1;
}

//
//
//(self, css)
static int ApplyCSS(lua_State * l)
{
    lua_settop(l, 2);

    auto css = lua_tostring(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
    if (bob)
        bob->applyCSS(css);
    return 0;
}

//(self, class, [mod])
static int ApplyClass(lua_State * l)
{
    lua_settop(l, 2);

    const char * class_name = lua_tostring(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    BonesObject * bob = LuaMetaTable::CallGetBonesObject(l);
    if (bob)
        bob->applyClass(class_name);
    return 0;
}



static int GetCObject(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushlightuserdata(l, lua_touserdata(l, lua_upvalueindex(1)));

    return 1;
}

//(self interval due [run stop start  pause resume])
static int Animate(lua_State * l)
{
    lua_settop(l, 8);
    lua_pushnil(l);

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    auto bo = LuaMetaTable::CallGetBonesObject(l);
    uint64_t interval = lua_tointeger(l, 2);
    uint64_t due = lua_tointeger(l, 3);

    auto ani = GetCoreInstance()->createAnimate(
        bo, interval, due, 0, 0, 0, 0, 0, kANI_SCRIPT);

    LuaContext::GetLOFromCO(l, ani);
    lua_pushnil(l);
    lua_copy(l, 4, -1);
    lua_setfield(l, -2, kMethodAnimateRun);
    lua_pushnil(l);
    lua_copy(l, 5, -1);
    lua_setfield(l, -2, kMethodAnimateStop);
    lua_pushnil(l);
    lua_copy(l, 6, -1);
    lua_setfield(l, -2, kMethodAnimateStart);
    lua_pushnil(l);
    lua_copy(l, 7, -1);
    lua_setfield(l, -2, kMethodAnimatePause);
    lua_pushnil(l);
    lua_copy(l, 8, -1);
    lua_setfield(l, -2, kMethodAnimateResume);

    GetCoreInstance()->startAnimate(ani);

    return 1;
}

void LuaMetaTable::CreatLuaTable(lua_State * l, const char * meta, BonesObject * bob)
{
    if (!bob)
        return;

    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, bob);
    lua_newtable(l);//1

    luaL_getmetatable(l, meta);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        luaL_newmetatable(l, meta);
        lua_pushnil(l);
        lua_copy(l, -2, -1);
        lua_setfield(l, -2, kMethodIndex);
        lua_pushcfunction(l, &GC);
        lua_setfield(l, -2, kMethodGC);

        lua_pushcfunction(l, &GetOpacity);
        lua_setfield(l, -2, kMethodGetOpacity);

        lua_pushcfunction(l, &GetLoc);
        lua_setfield(l, -2, kMethodGetLoc);

        lua_pushcfunction(l, &GetSize);
        lua_setfield(l, -2, kMethodGetSize);

        lua_pushcfunction(l, &Contains);
        lua_setfield(l, -2, kMethodContains);

        lua_pushcfunction(l, &GetChildAt);
        lua_setfield(l, -2, kMethodGetChildAt);

        //css method
        lua_pushcfunction(l, &ApplyCSS);
        lua_setfield(l, -2, kMethodApplyCSS);
        lua_pushcfunction(l, &ApplyClass);
        lua_setfield(l, -2, kMethodApplyClass);
        //animate method
        lua_pushcfunction(l, &Animate);
        lua_setfield(l, -2, kMethodAnimate);
        //lua_pushcfunction(l, &StopAnimate);
        //lua_setfield(l, -2, kMethodStop);
        //lua_pushcfunction(l, &PauseAnimate);
        //lua_setfield(l, -2, kMethodPause);
        //lua_pushcfunction(l, &ResumeAnimate);
        //lua_setfield(l, -2, kMethodResume);
        //lua_pushcfunction(l, &StopAllAnimate);
        //lua_setfield(l, -2, kMethodStopAll);
    }
    lua_setmetatable(l, -2);
    //lua table增加引用计数
    bob->retain();
    lua_pushstring(l, kMethodGetCObject);
    lua_pushlightuserdata(l, bob);
    lua_pushcclosure(l, &GetCObject, 1);
    lua_settable(l, -3);

    lua_settable(l, -3);
    lua_pop(l, 1);
}

void LuaMetaTable::RemoveLuaTable(lua_State * l, BonesObject * bob)
{
    if (!bob)
        return;

    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, bob);
    lua_pushnil(l);//1
    lua_settable(l, -3);
    lua_pop(l, 1);
}
//调用LO的GetCObject
BonesObject * LuaMetaTable::CallGetBonesObject(lua_State *l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, -1);
    assert(lua_istable(l, -1));
    lua_getfield(l, -1, kMethodGetCObject);
    assert(lua_isfunction(l, -1));
    if (!lua_isfunction(l, -1))
        return nullptr;
    lua_pushnil(l);
    lua_copy(l, -3, -1);
    auto ret = LuaContext::SafeLOPCall(l, 1, 1);
    assert(1 == ret);
    BonesObject * ud = (BonesObject *)lua_touserdata(l, -1);
    lua_pop(l, ret + 1);
    assert(ud);
    return ud;
}

/*
event
*/
static int StopPropagation(lua_State * l)
{
    Event * e = *(Event **)lua_touserdata(l, 1);
    if (e)
        e->stopPropagation();
    return 0;
}

static int PreventDefault(lua_State * l)
{
    Event * e = *(Event **)lua_touserdata(l, 1);
    if (e)
        e->preventDefault();
    return 0;
}

static int Canceled(lua_State * l)
{
    Event * e = *(Event **)lua_touserdata(l, 1);
    if (e)
        e->canceled();
    return 0;
}

static int Target(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    Event * e = *(Event **)lua_touserdata(l, 1);
    View * target = nullptr;
    if (e)
        target = e->target();
    if (target)
        LuaContext::GetLOFromCO(l, GetCoreInstance()->getObject(target));

    return 1;
}

static int Type(lua_State * l)
{
    Event * e = *(Event **)lua_touserdata(l, 1);
    const char * desc = nullptr;
    if (e)
        lua_pushinteger(l, e->type());
    else
        lua_pushnil(l);
    return 1;
}

static int Phase(lua_State * l)
{
    Event * e = *(Event **)lua_touserdata(l, 1);
    const char * desc = nullptr;
    if (e)
        lua_pushinteger(l, e->phase());
    else
        lua_pushnil(l);
    return 1;
}
/*
ui event
*/
static int IsShiftDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isShiftDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsControlDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isControlDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsCapsLockDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isCapsLockDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsAltDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isAltDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsAltGrDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isAltGrDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsCommandDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isCommandDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsLeftMouseDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isLeftMouseDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsMiddleMouseDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isMiddleMouseDown();

    lua_pushboolean(l, bret);
    return 1;
}

static int IsRightMouseDown(lua_State * l)
{
    bool bret = false;
    UIEvent * e = *(UIEvent **)lua_touserdata(l, 1);
    if (e)
        bret = e->isRightMouseDown();

    lua_pushboolean(l, bret);
    return 1;
}

/*............................................................
mouse event
*/
static int Loc(lua_State * l)
{
    MouseEvent * e = *(MouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    Scalar x = 0;
    Scalar y = 0;
    if (e)
    {
        x = e->getLoc().x();
        y = e->getLoc().y();
    }
    lua_pushnumber(l, x);
    lua_pushnumber(l, y);
    return 2;
}

static int RootLoc(lua_State *l)
{
    MouseEvent * e = *(MouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    Scalar x = 0;
    Scalar y = 0;
    if (e)
    {
        x = e->getRootLoc().x();
        y = e->getRootLoc().y();
    }
    lua_pushnumber(l, x);
    lua_pushnumber(l, y);
    return 2;
}

static int IsLeftMouse(lua_State * l)
{
    bool ret = false;
    MouseEvent * e = *(MouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    if (e)
        ret = e->isLeftMouse();
    lua_pushboolean(l, ret);
    return 1;
}

static int IsMiddleMouse(lua_State * l)
{
    bool ret = false;
    MouseEvent * e = *(MouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    if (e)
        ret = e->isMiddleMouse();
    lua_pushboolean(l, ret);
    return 1;
}

static int IsRightMouse(lua_State * l)
{
    bool ret = false;
    MouseEvent * e = *(MouseEvent **)luaL_checkudata(l, 1, kMetaTableMouseEvent);
    if (e)
        ret = e->isRightMouse();
    lua_pushboolean(l, ret);
    return 1;
}

/*............................................................
focus event
*/
static int IsTabTraversal(lua_State * l)
{
    bool ret = false;
    FocusEvent * e = *(FocusEvent **)luaL_checkudata(l, 1, kMetaTableFocusEvent);
    if (e)
        ret = e->isTabTraversal();
    lua_pushboolean(l, ret);
    return 1;
}

void LuaMetaTable::GetMouseEvent(lua_State * l)
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
}

void LuaMetaTable::GetFocusEvent(lua_State * l)
{
    GetUIEvent(l, kMetaTableFocusEvent);
    lua_pushcfunction(l, &IsTabTraversal);
    lua_setfield(l, -2, kMethodIsTabTraversal);
}

void LuaMetaTable::GetUIEvent(lua_State * l, const char * class_name)
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

void LuaMetaTable::GetEvent(lua_State * l, const char * class_name)
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
        lua_pushcfunction(l, &Type);
        lua_setfield(l, -2, kMethodType);
        lua_pushcfunction(l, &Phase);
        lua_setfield(l, -2, kMethodPhase);
    }
}


}