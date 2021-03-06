﻿#include "lua_text.h"
#include "lua_context.h"
#include "lua_check.h"
#include "utils.h"

#include "core/text.h"
#include "core/encoding.h"

namespace bones
{

static const char * kMetaTableText = "__mt_text";

static const char * kMethodSetFont = "setFont";
static const char * kMethodSetColor = "setColor";
static const char * kMethodSetContent = "setContent";
static const char * kMethodSetLineSpace = "setLineSpace";
static const char * kMethodSetAuto = "setAuto";
static const char * kMethodSetFloat = "setFloat";
static const char * kMethodSetPos = "setPos";
static const char * kMethodSetPath = "setPath";

static const char * kMethodGetFloatBounds = "getFloatBounds";

static int SetFont(lua_State * l)
{
    lua_settop(l, 7);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
    {
        BonesFont font;
        font.family = lua_tostring(l, 2);
        font.size = Utils::ToBonesScalar(lua_tonumber(l, 3));
        font.bold = !!lua_toboolean(l, 4);
        font.italic = !!lua_toboolean(l, 5);
        font.underline = !!lua_toboolean(l, 6);
        font.strike = !!lua_toboolean(l, 7);
        text->setFont(font);
    }
    return 0;
}

static int SetColor(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
    {
        if (lua_isinteger(l, 2))
            text->setColor(static_cast<BonesColor>(lua_tointeger(l, 2)));
        else if (lua_islightuserdata(l, 2))
            text->setColor(static_cast<BonesShader>(lua_touserdata(l, 2)));
    }
    return 0;
}

static int SetContent(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
    {
        const char * utf8 = lua_tostring(l, 2);
        if (utf8)
            text->setContent(Encoding::FromUTF8(utf8).data());
        else
            text->setContent(nullptr);
    }
    return 0;
}

static int SetLineSpace(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
        text->setLineSpace(Utils::ToBonesScalar(lua_tonumber(l, 2)));
    return 0;
}

static int SetAuto(lua_State * l)
{
    lua_settop(l, 3);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
    {      
        auto align = static_cast<BonesText::Align>(lua_tointeger(l, 2));
        auto ellipsis = !!(lua_toboolean(l, 3));
        text->setAuto(align, ellipsis);
    }    
    return 0;
}

static int SetFloat(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
        text->setFloat();
    return 0;
}
static int SetPos(lua_State * l)
{
    auto count = lua_gettop(l);

    if (count > 2)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaText * text = static_cast<LuaText *>(
            LuaContext::CallGetCObject(l));
        if (text)
        {
                std::vector<BonesPoint> bps;
                size_t pos_count = static_cast<size_t>(lua_tointeger(l, 2));
                lua_settop(l, pos_count * 2 + 2);

                for (int i = 3; i <= count; i = i + 2)
                {
                    BonesPoint bp ={ 
                        Utils::ToBonesScalar(lua_tonumber(l, i)),
                        Utils::ToBonesScalar(lua_tonumber(l, i + 1))
                    };
                    bps.push_back(bp);
                }
                text->setPos(pos_count, &bps[0]);
        }
    }

    return 0;
}

static int SetPath(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
        text->setPath(lua_touserdata(l, 2));
    return 0;
}

static int GetFloatBounds(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_pushnil(l);
    lua_pushnil(l);
    lua_pushnil(l);

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
    {
        auto r = text->getFloatBounds(Utils::ToBonesScalar(lua_tonumber(l, 2)));
        lua_pushnumber(l, r.left);
        lua_pushnumber(l, r.top);
        lua_pushnumber(l, r.right);
        lua_pushnumber(l, r.bottom);
    }
    return 4;
}

LuaText::LuaText(Text * ob)
:LuaObject(ob)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaText::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableText))
    {
        lua_pushcfunction(l, &SetFont);
        lua_setfield(l, -2, kMethodSetFont);

        lua_pushcfunction(l, &SetColor);
        lua_setfield(l, -2, kMethodSetColor);

        lua_pushcfunction(l, &SetLineSpace);
        lua_setfield(l, -2, kMethodSetLineSpace);

        lua_pushcfunction(l, &SetContent);
        lua_setfield(l, -2, kMethodSetContent);

        lua_pushcfunction(l, &SetAuto);
        lua_setfield(l, -2, kMethodSetAuto);

        lua_pushcfunction(l, &SetFloat);
        lua_setfield(l, -2, kMethodSetFloat);

        lua_pushcfunction(l, &SetPos);
        lua_setfield(l, -2, kMethodSetPos);

        lua_pushcfunction(l, &SetPath);
        lua_setfield(l, -2, kMethodSetPath);

        lua_pushcfunction(l, &GetFloatBounds);
        lua_setfield(l, -2, kMethodGetFloatBounds);

        
        
    }
}

void LuaText::setFont(const BonesFont & font)
{
    object_->setFont(ToFont(font));
}

void LuaText::setColor(BonesColor color)
{
    object_->setColor(color);
}

void LuaText::setColor(BonesShader shader)
{
    object_->setColor(static_cast<SkShader *>(shader));
}

void LuaText::setContent(const wchar_t * str)
{
    object_->set(str);
}

void LuaText::setLineSpace(BonesScalar ls)
{
    object_->setLineSpace(ls);
}

void LuaText::setAuto(Align align, bool ellipsis)
{
    Text::Align talign = Text::kCenter;
    if (kLeft == align)
        talign = Text::kLeft;
    else if (kRight == align)
        talign = Text::kRight;

    object_->setAuto(talign, ellipsis);
}

void LuaText::setPath(BonesPath path)
{
    object_->setPath(*Utils::ToSkPath(path));
}

void LuaText::setPos(size_t count, const BonesPoint * pts)
{
    if (!pts)
        object_->setPos(nullptr, 0);
    else
    {
        std::vector<Point> ps;
        for (size_t i = 0; i < count; ++i)
            ps.push_back(Point::Make(pts[i].x, pts[i].y));

        object_->setPos(&ps[0], count);
    }
}

void LuaText::setFloat()
{
    object_->setFloat();
}

BonesRect LuaText::getFloatBounds(BonesScalar max_width) const
{
    return Utils::ToBonesRect(object_->getFloatBounds(max_width));
}

}