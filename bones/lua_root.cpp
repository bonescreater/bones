#include "lua_root.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/root.h"
#include "core/helper.h"
#include "lua_meta_table.h"

namespace bones
{

static const char * kMetaTableRoot = "__mt_root";

LuaRoot::LuaRoot(Root * ob)
:LuaObject(ob)
{
    LuaMetaTable::CreatLuaTable(LuaContext::State(), kMetaTableRoot, this);
}

HDC LuaRoot::dc() const
{
    object_->getBackBuffer();
    return Helper::ToHDC(object_->getBackBuffer());
}

}