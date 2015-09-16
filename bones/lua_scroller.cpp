#include "lua_scroller.h"

namespace bones
{

static const char * kMetaTableScroller = "__mt_scroller";
static const char * kMethodonScrollRange = "onScrollRange";
static const char * kMethodonScrollPos = "onScrollPos";

LuaScroller::LuaScroller(Scroller * ob)
:LuaObject(ob), listener_(nullptr)
{
    createLuaTable();
}

LuaScroller::~LuaScroller()
{
    ;
}

LuaScroller::NotifyListener * LuaScroller::getNotify() const
{
    return listener_;
}

void LuaScroller::notifyCreate()
{
    object_->setDelegate(this);
    LuaObject::notifyCreate();
}

void LuaScroller::notifyDestroy()
{
    LuaObject::notifyDestroy();
    object_->setDelegate(nullptr);
}

void LuaScroller::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableScroller))
    {
        ;
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

void LuaScroller::setListener(NotifyListener * listener)
{
    listener_ = listener;
}

void LuaScroller::onScrollRange(Scroller * sender,
                                Scalar min_pos,
                                Scalar max_pos,
                                Scalar view_port,
                                bool horizontal)
{
    bool stop = false;
    listener_ ? listener_->onScrollRange(
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
    listener_ ? listener_->onScrollPos(this, cur_pos, horizontal) : 0;
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

}