#include "lua_text.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/text.h"
namespace bones
{

static const char * kMetaTableText = "__mt_text";

LuaText::LuaText(Text * ob)
:LuaObject(ob), notify_(nullptr)
{
    createLuaTable();
}

BonesText::NotifyListener * LuaText::getNotify() const
{
    return notify_;
}

void LuaText::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableText);
}

void LuaText::setListener(NotifyListener * lis)
{
    notify_ = lis;

}
}