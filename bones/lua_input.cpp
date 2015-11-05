#include "lua_input.h"
#include "lua_context.h"
#include "lua_check.h"
#include "utils.h"

#include "core/encoding.h"

namespace bones
{

static const char * kMetaTableText = "__mt_input";

static const char * kMethodSetFont = "setFont";
static const char * kMethodSetColor = "setColor";
static const char * kMethodSetContent = "setContent";

static int SetFont(lua_State * l)
{
    lua_settop(l, 7);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaInput * input = static_cast<LuaInput *>(
        LuaContext::CallGetCObject(l));
    if (input)
    {
        BonesFont font;
        font.family = lua_tostring(l, 2);
        font.size = Utils::ToBonesScalar(lua_tonumber(l, 3));
        font.bold = !!lua_toboolean(l, 4);
        font.italic = !!lua_toboolean(l, 5);
        font.underline = !!lua_toboolean(l, 6);
        font.strike = !!lua_toboolean(l, 7);
        input->setFont(font);
    }
    return 0;
}

static int SetColor(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaInput * input = static_cast<LuaInput *>(
        LuaContext::CallGetCObject(l));
    if (input)
    {
        if (lua_isinteger(l, 2))
            input->setColor(static_cast<BonesColor>(lua_tointeger(l, 2)));
        else if (lua_islightuserdata(l, 2))
            input->setColor(static_cast<BonesShader>(lua_touserdata(l, 2)));
    }
    return 0;
}

static int SetContent(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaInput * input = static_cast<LuaInput *>(
        LuaContext::CallGetCObject(l));
    if (input)
    {
        const char * utf8 = lua_tostring(l, 2);
        if (utf8)
            input->setContent(Encoding::FromUTF8(utf8).data());
        else
            input->setContent(nullptr);
    }
    return 0;
}

LuaInput::LuaInput(Input * ob)
    :LuaObject(ob)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaInput::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableText))
    {
        lua_pushcfunction(l, &SetFont);
        lua_setfield(l, -2, kMethodSetFont);

        lua_pushcfunction(l, &SetColor);
        lua_setfield(l, -2, kMethodSetColor);

        lua_pushcfunction(l, &SetContent);
        lua_setfield(l, -2, kMethodSetContent);
    }
}

void LuaInput::setFont(const BonesFont & font)
{
    object_->setFont(ToFont(font));
}

void LuaInput::setColor(BonesColor color)
{
    object_->setColor(color);
}

void LuaInput::setColor(BonesShader shader)
{
    object_->setColor(static_cast<SkShader *>(shader));
}

void LuaInput::setContent(const wchar_t * str)
{
    object_->set(str);
}

void LuaInput::setPassword(bool pw, wchar_t password)
{
    object_->setPassword(pw, password);
}

}