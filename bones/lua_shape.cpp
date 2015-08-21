#include "lua_shape.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/shape.h"
#include "core/logging.h"

namespace bones
{
static const char * kMetaTableShape = "__mt_shape";

LuaShape::LuaShape(Shape * co)
:LuaObject(co)
{
    LuaMetaTable::CreatLuaTable(LuaContext::State(), kMetaTableShape, this);
}
}
