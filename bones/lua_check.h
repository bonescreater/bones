#ifndef BONES_LUA_CHECK_H_
#define BONES_LUA_CHECK_H_

#include "bones.h"
#include "lua.hpp"
namespace bones
{

class LuaStackAutoCheck
{
public:
    LuaStackAutoCheck(lua_State * l, int count = 0);

    ~LuaStackAutoCheck();
private:
    lua_State * l_;
    int top_;
    int count_;
};

#define LUA_STACK_AUTO_CHECK(l) LuaStackAutoCheck __##__LINE__(l)

#define LUA_STACK_AUTO_CHECK_COUNT(l, a) LuaStackAutoCheck __##__LINE__(l, a)
}

#endif