#include "lua_shape.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/shape.h"
#include "core/logging.h"

namespace bones
{
static const char * kMetaTableShape = "__mt_shape";

LuaShape::LuaShape(Shape * co)
:LuaObject(co), notify_(nullptr)
{
    createLuaTable();
}

BonesShape::NotifyListener * LuaShape::getNotify() const
{
    return notify_;
}

void LuaShape::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableShape);
}

void LuaShape::setListener(NotifyListener * lis) 
{
    notify_ = lis;
}


}
