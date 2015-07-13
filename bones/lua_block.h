#ifndef BONES_LUA_BLOCK_H_
#define BONES_LUA_BLOCK_H_

#include "bones.h"

namespace bones
{
class Block;
class LuaBlock
{
public:
    static void Create(Block * block);
};

}

#endif