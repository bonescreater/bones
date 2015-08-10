#include "lua_rich_edit.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/logging.h"
#include "core/rich_edit.h"

namespace bones
{


void LuaRichEdit::Create(RichEdit * co)
{
    if (!co)
        return;
    if (kClassRichEdit != co->getClassName())
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
    LuaMetaTable::GetRichEdit(l);
    lua_setmetatable(l, -2);
    LuaMetaTable::SetClosureCObject(l, co);

    lua_settable(l, -3);
    lua_pop(l, 1);
}

}