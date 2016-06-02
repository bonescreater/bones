#ifndef BONES_BINDING_ENGINE_CONTEXT_H_
#define BONES_BINDING_ENGINE_CONTEXT_H_

#include "bones.h"
#include "lua.hpp"

namespace bones
{

class EngineContext
{
public:
    virtual lua_State * State() = 0;
};

}

#endif