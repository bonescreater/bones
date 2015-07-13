#ifndef BONES_LUA_CONTEXT_H_
#define BONES_LUA_CONTEXT_H_

#include "bones.h"

namespace bones
{
class LuaContext
{
public:
    static bool StartUp();

    static void ShutDown();

    static void Reset();

    static lua_State * State(); 

    static void GetGlobalTable(lua_State * l);

    static void GetCO2LOTable(lua_State * l);

    static void GetEventCache(lua_State * l);

    static int SafeLOPCall(lua_State * l, int nargs, int nresults);

    static void GetLOFromCO(lua_State * l, void * co);
};

}

#endif