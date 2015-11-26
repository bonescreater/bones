#ifndef BONES_LUA_INPUT_H_
#define BONES_LUA_INPUT_H_

#include "lua_handler.h"
#include "core/input.h"

namespace bones
{

class LuaInput : public LuaObject<BonesInput, Input>,
                 public Input::Delegate
{
public:
    LuaInput(Input * ob);

    void createMetaTable(lua_State * l) override;

    void setFont(const BonesFont & font) override;

    void setColor(BonesColor color) override;

    void setColor(BonesShader shader) override;

    void setContent(const wchar_t * str) override;

    void setPassword(bool pw, wchar_t password) override;

    void moveCaret(int index) override;
    
    void select(int start, int end) override;

    LUA_HANDLER(Input);
};

}

#endif