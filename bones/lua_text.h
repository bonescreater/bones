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

    void setFont(const BonesFont & font) override;

    void setColor(BonesColor color) override;

    void setColor(BonesShader shader) override;

    void setAutoContent(const wchar_t * str, Align align, OverFlow of) override;

    void setPosContent(const wchar_t * str, const BonesPoint * pts) override;

    LUA_HANDLER(Text);
};

}


#endif