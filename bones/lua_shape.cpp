#include "lua_shape.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/shape.h"
#include "core/logging.h"

namespace bones
{

void LuaShape::Create(Shape * co)
{
    if (!co)
        return;
    if (kClassShape != co->getClassName())
    {
        LOG_ERROR << co << "class name " << co->getClassName();
        return;
    }
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_newtable(l);//1

    LuaMetaTable::GetShape(l);
    lua_setmetatable(l, -2);
    LuaMetaTable::SetClosureCObject(l, co);

    lua_settable(l, -3);
    lua_pop(l, 1);
}
}
