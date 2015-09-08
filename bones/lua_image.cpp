#include "lua_image.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/image.h"

namespace bones
{

static const char * kMetaTableImage = "__mt_image";

LuaImage::LuaImage(Image * ob)
:LuaObject(ob, kMetaTableImage), notify_(nullptr)
{
    ;
}

BonesImage::NotifyListener * LuaImage::getNotify() const
{
    return notify_;
}

void LuaImage::setListener(NotifyListener * lis)
{
    notify_ = lis;
}

}