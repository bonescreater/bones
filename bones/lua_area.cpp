#include "lua_area.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_event.h"
#include "lua_meta_table.h"
#include "core/area.h"
#include "core/logging.h"

namespace bones
{

static const char * kMethodOnMouseDown = "onMouseDown";
static const char * kMethodOnMouseUp = "onMouseUp";
static const char * kMethodOnMouseMove = "onMouseMove";
static const char * kMethodOnMouseLeave = "onMouseLeave";
static const char * kMethodOnMouseEnter = "onMouseEnter";
static const char * kMethodOnSizeChanged = "onSizeChanged";

static EventType ToEventType(const char * name)
{
    assert(name);
    if (!name)
        return kET_CUSTOM;

    if (!strcmp(kMethodOnMouseDown, name))
        return kET_MOUSE_DOWN;
    if (!strcmp(kMethodOnMouseUp, name))
        return kET_MOUSE_UP;
    if (!strcmp(kMethodOnMouseMove, name))
        return kET_MOUSE_MOVE;
    if (!strcmp(kMethodOnMouseLeave, name))
        return kET_MOUSE_LEAVE;
    if (!strcmp(kMethodOnMouseEnter, name))
        return kET_MOUSE_ENTER;
    assert(0);
    LOG_VERBOSE << "unknown event type: " << name;
    return kET_CUSTOM;
}

static void PostToLO(Ref * co, Event & e, const char * method_name)
{
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);

    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_gettable(l, -2);
    assert(lua_istable(l, -1));
    lua_pushinteger(l, e.phase());
    lua_gettable(l, -2);
    assert(lua_istable(l, -1));

    lua_getfield(l, -1, method_name);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    LuaEvent::Get(l, e);
    LuaContext::SafeLOPCall(l, 2, 0);
    lua_pop(l, 1);

    lua_pop(l, 2);
}

static void PostToLO(Ref * co, const char * method_name)
{
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_gettable(l, -2);
    assert(lua_istable(l, -1));
    lua_getfield(l, -1, method_name);
    lua_pushnil(l);
    lua_copy(l, -3, -1);
    LuaContext::SafeLOPCall(l, 1, 0);
    lua_pop(l, 2);
}

static void OnEvent(Ref * sender, Event & e)
{
    const char * method = nullptr;
    switch (e.type())
    {
    case kET_MOUSE_ENTER:
        method = kMethodOnMouseEnter;
        break;
    case kET_MOUSE_MOVE:
        method = kMethodOnMouseMove;
        break;
    case kET_MOUSE_DOWN:
        method = kMethodOnMouseDown;
        break;
    case kET_MOUSE_UP:
        method = kMethodOnMouseUp;
        break;
    case kET_MOUSE_CLICK:
        break;
    case kET_MOUSE_DCLICK:
        break;
    case kET_MOUSE_LEAVE:
        method = kMethodOnMouseLeave;
        break;
    case kET_MOUSE_WHEEL:
        break;
    case kET_KEY_DOWN:
        break;
    case kET_KEY_UP:
        break;
    case kET_FOCUS_OUT:
        break;
    case kET_FOCUS_IN:
        break;
    case kET_BLUR:
        break;
    case kET_FOCUS:
        break;
    default:
        break;
    }
    if (method)
        PostToLO(sender, e, method);
}

static void OnSizeChanged(Ref * sender)
{
    PostToLO(sender, kMethodOnSizeChanged);
}

void LuaArea::Create(Area * co)
{
    if (!co)
        return;
    if (kClassArea != co->getClassName())
    {
        LOG_ERROR << co << "class name " << co->getClassName();
        return;
    }
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_newtable(l);//1
    //lua->c
    LuaMetaTable::GetArea(l);
    lua_setmetatable(l, -2);
    LuaMetaTable::SetClosureCObject(l, co);
    lua_pushinteger(l, Event::kCapture);
    lua_newtable(l);
    lua_settable(l, -3);
    lua_pushinteger(l, Event::kTarget);
    lua_newtable(l);
    lua_settable(l, -3);
    lua_pushinteger(l, Event::kBubbling);
    lua_newtable(l);
    lua_settable(l, -3);

    lua_settable(l, -3);
    lua_pop(l, 1);
}

void LuaArea::RegisterEvent(
    Area * co,
    const char * name,
    const char * phase,
    const char * module,
    const char * func)
{
    if (!name || !module || !func)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);

    lua_getglobal(l, module);
    //{mod} 入栈
    assert(lua_istable(l, -1));
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        LOG_VERBOSE << "require mod fail: " << module;
        return;
    }
    lua_getfield(l, -1, func);
    assert(lua_isfunction(l, -1));
    if (!lua_isfunction(l, -1))
    {
        lua_pop(l, 2);
        LOG_VERBOSE << "not function: " << func;
        return;
    }

    LuaContext::GetLOFromCO(l, co);
    if (lua_istable(l, -1))
    {
        Event::Phase ephase = Event::kTarget;
        if (phase)
        {
            if (!strcmp(kStrCapture, phase))
                ephase = Event::kCapture;
            else if (!strcmp(kStrTarget, phase))
                ephase = Event::kTarget;
            else if (!strcmp(kStrBubbling, phase))
                ephase = Event::kBubbling;
            else
                LOG_VERBOSE << "unknown event phase\n";
            lua_pushinteger(l, ephase);
            lua_gettable(l, -2);
            assert(lua_istable(l, -1));
            if (lua_istable(l, -1))
            {
                lua_pushnil(l);
                lua_copy(l, -4, -1);
                lua_setfield(l, -2, name);
            }
            lua_pop(l, 1);
            
            co->bind(ToEventType(name), ephase, BONES_CALLBACK_2(&OnEvent));
        }
        else
        {
            lua_pushnil(l);
            lua_copy(l, -3, -1);
            lua_setfield(l, -2, name);
            if (!strcmp(kMethodOnSizeChanged, name))
                co->bind(BONES_CALLBACK_1(&OnSizeChanged));
        }

    }
    lua_pop(l, 1);

    lua_pop(l, 2);
}


}