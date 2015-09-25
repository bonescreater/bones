#include "lua_rich_edit.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/rich_edit.h"

namespace bones
{

static const char * kMetaTableRichEdit = "__mt_richedit";
static const char * kMethodtxNotify = "txNotify";

LuaRichEdit::LuaRichEdit(RichEdit * ob)
:LuaObject(ob)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaRichEdit::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableRichEdit);
}

HRESULT LuaRichEdit::txNotify(RichEdit * sender, DWORD iNotify, void  *pv)
{
    bool stop = false;
    HRESULT result = notify_ ? notify_->txNotify(this, iNotify, pv, stop) : 0;
    if (stop)
        return result;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodtxNotify);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushinteger(l, iNotify);
        lua_pushinteger(l, (lua_Integer)pv);

        auto count = LuaContext::SafeLOPCall(l, 3, 1);
        if (count > 0)
            result = (HRESULT)lua_tointeger(l, -1);
        lua_pop(l, count);
    }
    lua_pop(l, 2);

    return result;
}



}