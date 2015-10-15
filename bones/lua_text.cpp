#include "lua_text.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/text.h"
#include "core/encoding.h"

namespace bones
{

static const char * kMetaTableText = "__mt_text";

static const char * kMethodSetFont = "setFont";
static const char * kMethodSetColor = "setColor";
static const char * kMethodSetAutoContent = "setAutoContent";
static const char * kMethodSetPosContent = "setPosContent";

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
        font.size = static_cast<BonesScalar>(lua_tonumber(l, 3));
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

static int SetAutoContent(lua_State * l)
{
    lua_settop(l, 4);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaText * text = static_cast<LuaText *>(
        LuaContext::CallGetCObject(l));
    if (text)
    {
        const char * utf8 = lua_tostring(l, 2);
        auto align = static_cast<BonesText::Align>(lua_tointeger(l, 3));
        auto of = static_cast<BonesText::OverFlow>(lua_tointeger(l, 4));     
        if (utf8)
        {
            auto str = Encoding::FromUTF8(utf8);
            text->setAutoContent(str.data(), align, of);
        }          
        else
            text->setAutoContent(nullptr, align, of);
    }    
    return 0;
}

static int SetPosContent(lua_State * l)
{
    auto count = lua_gettop(l);

    if (count >= 2)
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaText * text = static_cast<LuaText *>(
            LuaContext::CallGetCObject(l));
        if (text)
        {
            const char * utf8 = lua_tostring(l, 2);
            if (utf8)
            {
                auto str = Encoding::FromUTF8(utf8);
                std::vector<BonesPoint> bps;

                if (str.length() * 2 + 2 == count)
                {//pts和text匹配
                    for (int i = 3; i <= count; i = i + 2)
                    {
                        BonesPoint bp ={ 
                            static_cast<BonesScalar>(lua_tonumber(l, i)),
                            static_cast<BonesScalar>(lua_tonumber(l, i + 1))
                        };
                        bps.push_back(bp);
                    }
                    text->setPosContent(str.data(), &bps[0]);
                }
                else
                    BLG_ERROR << kMethodSetPosContent << " params invalid";
            }
            else
                text->setPosContent(nullptr, nullptr);
        }
    }

    return 0;
}

LuaText::LuaText(Text * ob)
:LuaObject(ob), notify_(nullptr)
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

        lua_pushcfunction(l, &SetAutoContent);
        lua_setfield(l, -2, kMethodSetAutoContent);

        lua_pushcfunction(l, &SetPosContent);
        lua_setfield(l, -2, kMethodSetPosContent);
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

void LuaText::setAutoContent(const wchar_t * str, Align align, OverFlow of)
{
    Text::Overflow tof = Text::kNone;
    if (kWordWrap == of)
        tof = Text::kWordWrap;
    else if (kEllipsis == of)
        tof = Text::kEllipsis;

    Text::Align talign = Text::kCenter;
    if (kLeft == align)
        talign = Text::kLeft;
    else if (kRight == align)
        talign = Text::kRight;

    object_->set(str, talign, tof);
}

void LuaText::setPosContent(const wchar_t * str, const BonesPoint * pts)
{
    if (!pts)
        object_->set(str, nullptr);
    else if (!str)
        object_->set(nullptr, nullptr);
    else
    {
        std::vector<Point> ps;
        auto length = wcslen(str);
        for (size_t i = 0; i < length; ++i)
            ps.push_back(Point::Make(pts[i].x, pts[i].y));

        object_->set(str, &ps[0]);
    }
}

}