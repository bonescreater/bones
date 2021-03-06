﻿#include "lua_scroller.h"
#include "utils.h"

namespace bones
{

static const char * kMetaTableScroller = "__mt_scroller";
static const char * kMethodonScrollRange = "onScrollRange";
static const char * kMethodonScrollPos = "onScrollPos";
static const char * kMethodsetScrollInfo = "setScrollInfo";
static const char * kMethodsetScrollPos = "setScrollPos";
static const char * kMethodGetScrollPos = "getScrollPos";
//(self, range, bool)
static int SetScrollInfo(lua_State * l)
{
    lua_settop(l, 3);
    lua_pushnil(l);
    lua_copy(l, 1, -1);

    LuaScroller * bob = static_cast<LuaScroller *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        bob->setScrollInfo(Utils::ToBonesScalar(lua_tonumber(l, 2)),
                           !!lua_toboolean(l, 3));

    return 0;
}

//(self, pos, bool)
static int SetScrollPos(lua_State * l)
{
    lua_settop(l, 3);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaScroller * bob = static_cast<LuaScroller *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        bob->setScrollPos(Utils::ToBonesScalar(lua_tonumber(l, 2)),
                          !!lua_toboolean(l, 3));
    return 0;
}
//(self, bool)
static int GetScrollPos(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaScroller * bob = static_cast<LuaScroller *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        lua_pushnumber(l, bob->getScrollPos(!!lua_toboolean(l, 2)));
    return 1;
}

LuaScroller::LuaScroller(Scroller * ob)
:LuaObject(ob)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaScroller::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableScroller))
    {
        lua_pushcfunction(l, &SetScrollInfo);
        lua_setfield(l, -2, kMethodsetScrollInfo);
        lua_pushcfunction(l, &SetScrollPos);
        lua_setfield(l, -2, kMethodsetScrollPos);
        lua_pushcfunction(l, &GetScrollPos);
        lua_setfield(l, -2, kMethodGetScrollPos);
    }
}

void LuaScroller::setScrollInfo(BonesScalar total, bool horizontal)
{
    object_->setScrollInfo(total, horizontal);
}

void LuaScroller::setScrollPos(BonesScalar cur, bool horizontal)
{
    object_->setScrollPos(cur, horizontal);
}

void LuaScroller::onScrollRange(Scroller * sender,
                                Scalar min_pos,
                                Scalar max_pos,
                                Scalar view_port,
                                bool horizontal)
{
    bool stop = false;
    notify_ ? notify_->onScrollRange(
        this, min_pos, max_pos, view_port, horizontal) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodonScrollRange);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, min_pos);
        lua_pushnumber(l, max_pos);
        lua_pushnumber(l, view_port);
        lua_pushboolean(l, horizontal);
        LuaContext::SafeLOPCall(l, 5, 0);
    }
    lua_pop(l, 2);
}

void LuaScroller::onScrollPos(Scroller * sender,
                              Scalar cur_pos,
                              bool horizontal)
{
    bool stop = false;
    notify_ ? notify_->onScrollPos(this, cur_pos, horizontal) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodonScrollPos);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, cur_pos);
        lua_pushboolean(l, horizontal);
        LuaContext::SafeLOPCall(l, 3, 0);
    }
    lua_pop(l, 2);
}

BonesScalar LuaScroller::getScrollPos(bool horizontal)
{
    return object_->getScrollPos(horizontal);
}

}