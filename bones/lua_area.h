#ifndef BONES_LUA_AREA_H_
#define BONES_LUA_AREA_H_

#include "bones.h"


namespace bones
{
class Area;

class LuaArea
{
public:
    static void Create(Area * co);

    static void RegisterEvent(Area * co,
                              const char * name,
                              const char * phase, 
                              const char * module, 
                              const char * func);
};

}
#endif