#include "lua_image.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/image.h"

namespace bones
{

static const char * kMetaTableImage = "__mt_image";

LuaImage::LuaImage(Image * ob)
:LuaObject(ob, kMetaTableImage)
{
    ;
}
}