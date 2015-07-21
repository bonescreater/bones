#include "lua_meta_table.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_animation.h"
#include "bones_export.h"

#include "core/panel.h"
#include "core/root_view.h"
#include "core/shirt.h"
#include "core/view.h"
#include "core/logging.h"
#include "core/text.h"
#include "core/encoding.h"
#include "core/animation_manager.h"

namespace bones
{
static const char * kMetaTablePanel = "__bone__panel__";
static const char * kMetaTableArea = "__bone__area__";
static const char * kMetaTableBlock = "__bone__block__";
static const char * kMetaTableText = "__bone__text__";
static const char * kMetaTableShape = "__bone__shape__";
static const char * kMetaTableMouseEvent = "__bone__mouse__event__";
static const char * kMetaTableFocusEvent = "__bone__focus__event__";

static const char * kMethodIndex = "__index";
static const char * kMethodGC = "__gc";
static const char * kMethodOpacity = "getOpacity";
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
static const char * kMethodSize = "getSize";

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

//调用LO的GetCObject
Ref * LuaMetaTable::CallGetCObject(lua_State *l)
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
    Ref * ud = (Ref *)lua_touserdata(l, -1);
    lua_pop(l, ret + 1);
    assert(ud);
    return ud;
}

static int Contains(lua_State * l)
{
    bool bret = false;
    int count = lua_gettop(l);
    if (count == 3)
    {
        float x = static_cast<float>(lua_tonumber(l, -2));
        float y = static_cast<float>(lua_tonumber(l, -1));
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        View * v = (View *)LuaMetaTable::CallGetCObject(l);
        if (v)
            bret = v->contains(Point::Make(x, y));
    }
    lua_pushboolean(l, bret);
    return 1;
}
//(self)
static int GetSize(lua_State * l)
{
    bool bret = false;
    int count = lua_gettop(l);
    lua_pushnil(l);
    lua_pushnil(l);
    if (count == 1)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        View * v = (View *)LuaMetaTable::CallGetCObject(l);
        lua_pushnumber(l, v->getWidth());
        lua_pushnumber(l, v->getHeight());
    }

    return 2;
}


//(self, css)
static int ApplyCSS(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 2)
    {
        auto css = lua_tostring(l, -1);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        Ref * v = (Ref *)LuaMetaTable::CallGetCObject(l);
        BonesApplyCSS(v, css);
    }
    else
        LOG_VERBOSE << "param count error\n";
    return 0;
}

//(self, class, [mod])
static int ApplyClass(lua_State * l)
{
    int count = lua_gettop(l);
    if (2 == count || 3 == count)
    {
        const char * class_name = lua_tostring(l, 2);
        const char * mod = nullptr;
        if (count == 3)
            mod = lua_tostring(l, 3);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        Ref * v = (Ref *)LuaMetaTable::CallGetCObject(l);
        BonesApplyClass(v, class_name, mod);
    }
    return 0;
}

//(self, index)
static int GetChildAt(lua_State * l)
{
    bool success = false;
    int count = lua_gettop(l);
    if (count == 2)
    {
        size_t index = static_cast<size_t>(lua_tonumber(l, -1));
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        View * v = (View *)LuaMetaTable::CallGetCObject(l);
        auto child = v->getChildAt(index);
        if (child)
        {
            LuaContext::GetLOFromCO(l, child);
            success = true;
        }           
    }

    if (!success)
        lua_pushnil(l);
    return 1;
}

//(self)
static int GC(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        Ref * ref = LuaMetaTable::CallGetCObject(l);
        if (ref)
            ref->release();
    }
    return 0;
}

static int Opacity(lua_State * l)
{
    int count = lua_gettop(l);
    lua_pushnil(l);
    if (count == 1)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        Ref * ref = LuaMetaTable::CallGetCObject(l);
        if (ref)
        {
            auto opa = 0.f;
            if (kClassPanel == ref->getClassName())
                opa = ((Panel *)ref)->getOpacity();
            else if (kClassRootView == ref->getClassName())
                opa = ((RootView *)ref)->getOpacity();
            else
                opa = ((Shirt *)ref)->getOpacity();
            lua_pushnumber(l, opa);
        }
    }
    return 1;
}

static int GetCObject(lua_State * l)
{
    int count = lua_gettop(l);
    lua_pushnil(l);
    if (count == 1)
    {
        Ref * ref = (Ref *)(lua_touserdata(l, lua_upvalueindex(1)));
        assert(ref);
        lua_pushlightuserdata(l, ref);
    }
    return 1;
}

//(self interval due [run start stop pause resume])
static int Animate(lua_State * l)
{
    int count = lua_gettop(l);
    lua_pushnil(l);
    if (count >= 3)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        Ref * ref = LuaMetaTable::CallGetCObject(l);
        uint64_t interval = lua_tointeger(l, 2);
        uint64_t due = lua_tointeger(l, 3);
        const char * run = nullptr;
        const char * run_module = nullptr;
        if (count >= 4)
            run = lua_tostring(l, 4);
        if (count >= 5)
            run_module = lua_tostring(l, 5);

        const char * stop = nullptr;
        const char * stop_module = nullptr;
        if (count >= 6)
            stop = lua_tostring(l, 6);
        if (count >= 7)
            stop_module = lua_tostring(l, 7);

        const char * start = nullptr;
        const char * start_module = nullptr;
        if (count >= 8)
            start = lua_tostring(l, 8);
        if (count >= 9)
            start_module = lua_tostring(l, 9);

        const char * pause = nullptr;
        const char * pause_module = nullptr;
        if (count >= 10)
            pause = lua_tostring(l, 10);
        if (count >= 11)
            pause_module = lua_tostring(l, 11);

        const char * resume = nullptr;
        const char * resume_module = nullptr;
        if (count >= 12)
            resume = lua_tostring(l, 12);
        if (count >= 13)
            resume_module = lua_tostring(l, 13);

        auto * ani = LuaAnimation::Create(ref, interval, due, 
            run, run_module, stop, stop_module, start, start_module,
            pause, pause_module, resume, resume_module);
        Core::GetAnimationManager()->add(ani);
        ani->release();
        lua_pushlightuserdata(l, ani);
    }
    return 1;
}
//(self ani)
static int StopAnimate(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 2)
    {
        auto ani = (Animation *)lua_touserdata(l, 2);
        Core::GetAnimationManager()->remove(ani);
    }
    return 0;
}
//(self ani)
static int PauseAnimate(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 2)
    {
        auto ani = (Animation *)lua_touserdata(l, 2);
        Core::GetAnimationManager()->pause(ani);
    }
    return 0;
}
//(self ani)
static int ResumeAnimate(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 2)
    {
        auto ani = (Animation *)lua_touserdata(l, 2);
        Core::GetAnimationManager()->resume(ani);
    }
    return 0;
}
//(self)
static int StopAllAnimate(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        Ref * ref = LuaMetaTable::CallGetCObject(l);
        Core::GetAnimationManager()->remove(ref);
    }
    return 0;
}

void LuaMetaTable::GetPanel(lua_State * l)
{
    GetRef(l, kMetaTablePanel);
}

void LuaMetaTable::GetArea(lua_State * l)
{
    GetView(l, kMetaTableArea);
}

void LuaMetaTable::GetBlock(lua_State * l)
{
    GetView(l, kMetaTableBlock);
}

void LuaMetaTable::GetText(lua_State * l)
{
    GetView(l, kMetaTableText);
}

void LuaMetaTable::GetShape(lua_State * l)
{
    GetView(l, kMetaTableShape);
}

void LuaMetaTable::GetRef(lua_State *l, const char * class_name)
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

        lua_pushcfunction(l, &GC);
        lua_setfield(l, -2, kMethodGC);

        //view panel
        lua_pushcfunction(l, &Opacity);
        lua_setfield(l, -2, kMethodOpacity);
        //css method
        lua_pushcfunction(l, &ApplyCSS);
        lua_setfield(l, -2, kMethodApplyCSS);
        lua_pushcfunction(l, &ApplyClass);
        lua_setfield(l, -2, kMethodApplyClass);
        //animate method
        lua_pushcfunction(l, &Animate);
        lua_setfield(l, -2, kMethodAnimate);
        lua_pushcfunction(l, &StopAnimate);
        lua_setfield(l, -2, kMethodStop);
        lua_pushcfunction(l, &PauseAnimate);
        lua_setfield(l, -2, kMethodPause);
        lua_pushcfunction(l, &ResumeAnimate);
        lua_setfield(l, -2, kMethodResume);
        lua_pushcfunction(l, &StopAllAnimate);
        lua_setfield(l, -2, kMethodStopAll);
    }
}

void LuaMetaTable::GetView(lua_State * l, const char * class_name)
{
    assert(l && class_name);
    GetRef(l, class_name);
    lua_pushcfunction(l, &Contains);
    lua_setfield(l, -2, kMethodContains);

    lua_pushcfunction(l, &GetChildAt);
    lua_setfield(l, -2, kMethodGetChildAt);

    lua_pushcfunction(l, &GetSize);
    lua_setfield(l, -2, kMethodSize);
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
    Event * e = *(Event **)lua_touserdata(l, 1);
    View * target = nullptr;
    if (e)
        target = e->target();
    if (target)
        LuaContext::GetLOFromCO(l, target);
    else
        lua_pushnil(l);
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

void LuaMetaTable::SetClosureCObject(lua_State * l, Ref * ref)
{
    assert(lua_istable(l, -1));
    LUA_STACK_AUTO_CHECK(l);
    if (!lua_istable(l, -1))
        return;
    ref->retain();
    lua_pushstring(l, kMethodGetCObject);
    lua_pushlightuserdata(l, ref);
    lua_pushcclosure(l, &GetCObject, 1);
    lua_settable(l, -3);
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