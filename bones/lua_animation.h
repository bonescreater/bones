#ifndef BONES_LUA_ANIMATION_H_
#define BONES_LUA_ANIMATION_H_

#include "bones.h"
#include "core/animation.h"

namespace bones
{

class LuaAnimation
{
public:
    static Animation * Create(Ref * target, uint64_t interval, uint64_t due,
                 const char * run,
                 const char * stop,
                 const char * start,
                 const char * pause,
                 const char * resume);
};

}

#endif