#ifndef BONES_LUA_TEXT_H_
#define BONES_LUA_TEXT_H_

#include "lua_object.h"

namespace bones
{
class Text;
class LuaText : public LuaObject<BonesText, Text>
{
public:
    LuaText(Text * ob);
};

}


#endif