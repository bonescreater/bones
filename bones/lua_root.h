#ifndef BONES_LUA_ROOT_H_
#define BONES_LUA_ROOT_H_

#include "lua_object.h"

namespace bones
{

class Root;

class LuaRoot : public LuaObject<BonesRoot, Root>
{
public:
    LuaRoot(Root *);

    HDC dc() const override;
};


}


#endif