#include "lua_panel.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/panel.h"
#include "core/logging.h"

namespace bones
{

void LuaPanel::Create(Panel * co)
{
    if (!co)
        return;
    if (kClassPanel != co->getClassName())
    {
        LOG_ERROR << co << "class name " << co->getClassName();
        return;
    }
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_newtable(l);//1

    LuaMetaTable::GetPanel(l);
    lua_setmetatable(l, -2);
    LuaMetaTable::SetClosureCObject(l, co);

    lua_settable(l, -3);
    lua_pop(l, 1);
}

void LuaPanel::RegisterEvent(Panel * co,
                             const char * name,
                             const char * phase,
                             const char * module,
                             const char * func)
{
    assert(!phase);
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
        lua_pushnil(l);
        lua_copy(l, -3, -1);
        lua_setfield(l, -2, name);
    }
    lua_pop(l, 1);

    lua_pop(l, 2);
}


}