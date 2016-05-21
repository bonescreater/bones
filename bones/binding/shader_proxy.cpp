#include "shader_proxy.h"
#include "SkGradientShader.h"
#include "lua_utils.h"
#include "lua_check.h"

#include <vector>

namespace bones
{

static const char * kProxy = "ShaderProxy";
static const char * kMethodCreateLinearGradient = "createLinearGradient";
static const char * kMethodCreateRadialGradient = "createRadialGradient";
static const char * kMethodRelease = "release";

static inline SkShader * Cast(void * shader)
{
    return static_cast<SkShader *>(shader);
}

static SkShader::TileMode Mode(const char * mode)
{
    SkShader::TileMode sm = SkShader::kClamp_TileMode;
    if (mode)
    {
        //"clamp"
        if (0 == strcmp("repeat", mode))
            sm = SkShader::kRepeat_TileMode;
        else if (0 == strcmp("mirror", mode))
            sm = SkShader::kMirror_TileMode;
    }

    return sm;
}

static int CreateLinearGradient(lua_State * l)
{//(begin, end, color, pos , mode)
    auto count = lua_gettop(l);
    bool bret = false;
    if (5 == count)
    {
        auto mode = Mode(lua_tostring(l, 5));
        lua_pop(l, 1);

        std::vector<BonesScalar> pos;
        std::vector<BonesColor> colors;
        auto pos_len = LuaUtils::ToInt(luaL_len(l, 4));
        auto color_len = LuaUtils::ToInt(luaL_len(l, 3));
        auto len = pos_len > color_len ? color_len : pos_len;
        if (len > 0)
        {
            colors.resize(len);
            pos.resize(len);
            LuaUtils::PopScalarArray(l, &pos[0], len);
            LuaUtils::PopColorArray(l, &colors[0], len);

            BonesPoint begin, end;
            LuaUtils::PopPoint(l, end);
            LuaUtils::PopPoint(l, begin);
            SkPoint point[2] = { { begin.x, begin.y }, { end.x, end.y } };
            lua_pushlightuserdata(l,
                SkGradientShader::CreateLinear(
                point, &colors[0], &pos[0], len, mode));
            bret = true;
        }
    }

    if (!bret)
        lua_pushnil(l);

    return 1;
}

static int CreateRadialGradient(lua_State * l)
{//(center, radius, colors, pos, mode)
    auto count = lua_gettop(l);
    bool bret = false;
    if (5 == count)
    {
        auto mode = Mode(lua_tostring(l, 5));
        lua_pop(l, 1);

        std::vector<BonesScalar> pos;
        std::vector<BonesColor> colors;
        auto pos_len = LuaUtils::ToInt(luaL_len(l, 4));
        auto color_len = LuaUtils::ToInt(luaL_len(l, 3));
        auto len = pos_len > color_len ? color_len : pos_len;
        if (len > 0)
        {
            colors.resize(len);
            pos.resize(len);
            LuaUtils::PopScalarArray(l, &pos[0], len);
            LuaUtils::PopColorArray(l, &colors[0], len);

            auto radius = LuaUtils::ToScalar(lua_tonumber(l, 2));
            lua_pop(l, 1);
            BonesPoint center;
            LuaUtils::PopPoint(l, center);
            SkPoint point = { center.x, center.y };
            lua_pushlightuserdata(l,
                SkGradientShader::CreateRadial(
                point, radius, &colors[0], &pos[0], len, mode));
            bret = true;
        }
    }

    if (!bret)
        lua_pushnil(l);

    return 1;
}

static int Release(lua_State * l)
{//shader
    lua_settop(l, 1);
    auto shader = Cast(lua_touserdata(l, 1));
    if (shader)
        shader->unref();

    return 0;
}


ShaderProxy::ShaderProxy(lua_State * s)
{
    state_ = s;
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_newtable(state_);

    lua_pushcfunction(state_, &CreateLinearGradient);
    lua_setfield(state_, -2, kMethodCreateLinearGradient);
    lua_pushcfunction(state_, &CreateRadialGradient);
    lua_setfield(state_, -2, kMethodCreateRadialGradient);
    lua_pushcfunction(state_, &Release);
    lua_setfield(state_, -2, kMethodRelease);

    lua_setfield(state_, -2, kProxy);
    LuaUtils::PopContext(state_);
}

ShaderProxy::~ShaderProxy()
{
    LuaUtils::PushContext(state_);
    lua_pushnil(state_);
    lua_setfield(state_, -2, kProxy);
    LuaUtils::PopContext(state_);
}

BonesShader ShaderProxy::createLinearGradient(
    const BonesPoint & begin,
    const BonesPoint & end,
    int len, BonesColor * color,
    BonesScalar * pos, const char * mode)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodCreateLinearGradient);
    LuaUtils::PushPoint(state_, begin);
    LuaUtils::PushPoint(state_, end);
    LuaUtils::PushColorArray(state_, color, len);
    LuaUtils::PushScalarArray(state_, pos, len);
    lua_pushstring(state_, nullptr == mode ? "":mode);

    auto count = LuaUtils::SafePCall(state_, 5);
    auto shader = lua_touserdata(state_, -1);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
    return shader;
}

BonesShader ShaderProxy::createRadialGradient(
    const BonesPoint & center,
    BonesScalar radius,
    int len, BonesColor * color,
    float * pos, const char * mode)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodCreateRadialGradient);
    LuaUtils::PushPoint(state_, center);
    lua_pushnumber(state_, radius);
    LuaUtils::PushColorArray(state_, color, len);
    LuaUtils::PushScalarArray(state_, pos, len);
    lua_pushstring(state_, nullptr == mode ? "": mode);

    auto count = LuaUtils::SafePCall(state_, 5);
    auto shader = lua_touserdata(state_, -1);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
    return shader;
}

void ShaderProxy::release(BonesShader shader)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodRelease);
    lua_pushlightuserdata(state_, shader);
    auto count = LuaUtils::SafePCall(state_, 1);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

}