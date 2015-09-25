#ifndef BONES_LUA_TEXT_H_
#define BONES_LUA_TEXT_H_

#include "lua_handler.h"
#include "core/text.h"

namespace bones
{
class Text;
class LuaText : public LuaObject<BonesText, Text>,
                public Text::Delegate
{
public:
    LuaText(Text * ob);

    void createMetaTable(lua_State * l) override;

    LUA_HANDLER(Text);
};

}


#endif