#ifndef BONES_LUA_AREA_H_
#define BONES_LUA_AREA_H_

#include "lua_object.h"


namespace bones
{
class Area;

class LuaArea :public LuaObject<BonesArea, Area>
{
public:
    LuaArea(Area * ob);

    static void RegisterEvent(Area * co,
                              const char * name,
                              const char * phase, 
                              const char * module, 
                              const char * func);
};

}
#endif