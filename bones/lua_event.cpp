#include "lua_event.h"
#include "lua_check.h"
#include "lua_context.h"
#include "lua_meta_table.h"
#include "core/event.h"
#include "core/logging.h"
namespace bones
{

void LuaEvent::Get(lua_State * l, Event & e)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);

    LuaContext::GetEventCache(l);
    void **userdata = (void **)lua_touserdata(l, -1);
    if (userdata == nullptr)
        return;

    *userdata = &e;
    if (MouseEvent::From(e))
        LuaMetaTable::GetMouseEvent(l);
    else if (FocusEvent::From(e))
        LuaMetaTable::GetFocusEvent(l);
    else
    {
        lua_pushnil(l);
        LOG_VERBOSE << "unsupport event metatable\n";
    }
        

    lua_setmetatable(l, -2);
}



}