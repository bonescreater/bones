#ifndef BONES_LUA_SHAPE_H_
#define BONES_LUA_SHAPE_H_

#include "lua_object.h"

namespace bones
{

class Shape;
class LuaShape : public LuaObject<BonesShape, Shape>
{
public:
    LuaShape(Shape * ob);
};

}

#endif