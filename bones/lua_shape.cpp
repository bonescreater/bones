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
:LuaObject(co, kMetaTableShape), notify_(nullptr)
{
    
}

BonesShape::NotifyListener * LuaShape::getNotify() const
{
    return notify_;
}

void LuaShape::setListener(NotifyListener * lis) 
{
    notify_ = lis;
}


}
