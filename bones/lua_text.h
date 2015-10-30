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

    void setContent(const wchar_t * str) override;

    void setLineSpace(BonesScalar ls) override;

    void setAuto(Align align, bool ellipsis) override;

    void setFloat() override;

    void setPos(size_t count, const BonesPoint * pts) override;

    void setPath(BonesPath path) override;

    BonesRect getFloatBounds(BonesScalar max_width) const override;

    LUA_HANDLER(Text);
};

}


#endif