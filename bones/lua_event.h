#ifndef BONES_LUA_EVENT_H_
#define BONES_LUA_EVENT_H_

#include "bones.h"
#include "lua_check.h"
#include "lua_context.h"

namespace bones
{

class Event;

class LuaEvent
{
public:
    static void Get(lua_State * l, Event & e);
};


}
#endif