#ifndef BONES_LUA_IMAGE_H_
#define BONES_LUA_IMAGE_H_

#include "lua_object.h"

namespace bones
{
class Image;
class LuaImage : public LuaObject<BonesImage, Image>
{
public:
    LuaImage(Image * ob);
};

}

#endif