#include "lua_root.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/root.h"
#include "core/helper.h"
#include "lua_meta_table.h"

namespace bones
{

static const char * kMetaTableRoot = "__mt_root";

static const char * kMethodRequestFocus = "requestFocus";
static const char * kMethodInvalidRect = "invalidRect";
static const char * kMethodChangeCursor = "changeCursor";
static const char * kMethodOnSizeChanged = "onSizeChanged";

LuaRoot::LuaRoot(Root * ob)
:LuaObject(ob), listener_(nullptr)
{
    ob->setDelegate(this);
    LuaMetaTable::CreatLuaTable(LuaContext::State(), kMetaTableRoot, this);
    //创建1个存放nofity的通知
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_newtable(l);
    lua_setfield(l, -2, kNotifyOrder);
    lua_pop(l, 1);
}

void LuaRoot::addNotify(const char * notify_name, const char * mod, const char * func)
{
    if (!notify_name || !func)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    lua_pushnil(l);
    //得到func
    if (mod)
    {
        lua_getglobal(l, mod);
        assert(lua_istable(l, -1));
        lua_getfield(l, -1, func);
        lua_copy(l, -1, -3);
        lua_pop(l, 2);
    }
    else
    {
        lua_getglobal(l, func);
        lua_copy(l, -1, -2);
        lua_pop(l, 1);
    }
    assert(lua_isfunction(l, -1));
    lua_setfield(l, -2, notify_name);
    lua_pop(l, 2);
}

void LuaRoot::setListener(Listener * listener)
{
    listener_ = listener;
}

bool LuaRoot::isDirty() const
{
    return object_->isDirty();
}

BonesRect LuaRoot::getDirtyRect() const
{   
    auto & ob = object_->getDirtyRect();
    BonesRect br = { ob.left(), ob.top(), ob.right(), ob.bottom() };
    return br;
}
void LuaRoot::draw()
{
    object_->draw();
}

HDC LuaRoot::dc() const
{
    object_->getBackBuffer();
    return Helper::ToHDC(object_->getBackBuffer());
}

void LuaRoot::requestFocus(Ref * sender)
{
    bool stop = false;
    listener_ ? listener_->requestFocus(this, stop): 0;
    if (stop)
        return;
    //post to script
    //(self)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    lua_getfield(l, -1, kMethodRequestFocus);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    LuaContext::SafeLOPCall(l, 1, 0);
    lua_pop(l, 2);
}

void LuaRoot::invalidRect(Ref * sender, const Rect & rect)
{
    bool stop = false;
    BonesRect r = { rect.left(), rect.top(), rect.right(), rect.bottom() };
    listener_ ? listener_->invalidRect(this, r, stop) : 0;
    if (stop)
        return;
    //(self, l, t, r, b)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    lua_getfield(l, -1, kMethodInvalidRect);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    lua_pushnumber(l, r.left);
    lua_pushnumber(l, r.top);
    lua_pushnumber(l, r.right);
    lua_pushnumber(l, r.bottom);
    LuaContext::SafeLOPCall(l, 5, 0);
    lua_pop(l, 2);
}

void LuaRoot::changeCursor(Ref * sender, Cursor cursor)
{
    bool stop = false;
    listener_ ? listener_->changeCursor(this, cursor, stop) : 0;
    if (stop)
        return;
    //(self, cursor)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    lua_getfield(l, -1, kMethodChangeCursor);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    lua_pushinteger(l, (lua_Integer)cursor);
    LuaContext::SafeLOPCall(l, 2, 0);
    lua_pop(l, 2);
}

void LuaRoot::onSizeChanged(Ref * sender, const Size & size)
{
    bool stop = false;
    BonesSize bs = { size.width(), size.height() };
    listener_ ? listener_->onSizeChanged(this, bs, stop) : 0;
    if (stop)
        return;
    //(self, w, h)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    lua_getfield(l, -1, kMethodOnSizeChanged);
    lua_pushnil(l);
    lua_copy(l, -4, -1);
    lua_pushnumber(l, bs.width);
    lua_pushnumber(l, bs.height);
    LuaContext::SafeLOPCall(l, 3, 0);
    lua_pop(l, 2);
}


}