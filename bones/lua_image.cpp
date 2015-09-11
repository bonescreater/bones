#include "lua_image.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/image.h"

namespace bones
{

static const char * kMetaTableImage = "__mt_image";

LuaImage::LuaImage(Image * ob)
:LuaObject(ob), notify_(nullptr)
{
    createLuaTable();
}

BonesImage::NotifyListener * LuaImage::getNotify() const
{
    return notify_;
}

void LuaImage::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableImage);
}

void LuaImage::setListener(NotifyListener * lis)
{
    notify_ = lis;
}

}