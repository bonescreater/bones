#include "lua_text.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/text.h"
namespace bones
{

static const char * kMetaTableText = "__mt_text";

LuaText::LuaText(Text * ob)
:LuaObject(ob, kMetaTableText), notify_(nullptr)
{
    ;
}

BonesText::NotifyListener * LuaText::getNotify() const
{
    return notify_;
}

void LuaText::setListener(NotifyListener * lis)
{
    notify_ = lis;

}
}