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
        auto pos_len = LuaUtils::IntFromLen(l, 4);
        auto color_len = LuaUtils::IntFromLen(l, 3);
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
        auto pos_len = LuaUtils::IntFromLen(l, 4);
        auto color_len = LuaUtils::IntFromLen(l, 3);
        auto len = pos_len > color_len ? color_len : pos_len;
        if (len > 0)
        {
            colors.resize(len);
            pos.resize(len);
            LuaUtils::PopScalarArray(l, &pos[0], len);
            LuaUtils::PopColorArray(l, &colors[0], len);

            auto radius = LuaUtils::ScalarFromNumber(l, 2);
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


ShaderProxy::ShaderProxy(EngineContext & ctx)
{
    ctx_ = &ctx;
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_newtable(l);

    lua_pushcfunction(l, &CreateLinearGradient);
    lua_setfield(l, -2, kMethodCreateLinearGradient);
    lua_pushcfunction(l, &CreateRadialGradient);
    lua_setfield(l, -2, kMethodCreateRadialGradient);
    lua_pushcfunction(l, &Release);
    lua_setfield(l, -2, kMethodRelease);

    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

ShaderProxy::~ShaderProxy()
{
    auto l = ctx_->State();
    LuaUtils::PushContext(l);
    lua_pushnil(l);
    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

BonesShader ShaderProxy::createLinearGradient(
    const BonesPoint & begin,
    const BonesPoint & end,
    int len, BonesColor * color,
    BonesScalar * pos, const char * mode)
{
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodCreateLinearGradient);
    LuaUtils::PushPoint(l, begin);
    LuaUtils::PushPoint(l, end);
    LuaUtils::PushColorArray(l, color, len);
    LuaUtils::PushScalarArray(l, pos, len);
    lua_pushstring(l, nullptr == mode ? "" : mode);

    auto count = LuaUtils::SafePCall(l, 5);
    auto shader = lua_touserdata(l, -1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
    return shader;
}

BonesShader ShaderProxy::createRadialGradient(
    const BonesPoint & center,
    BonesScalar radius,
    int len, BonesColor * color,
    float * pos, const char * mode)
{
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodCreateRadialGradient);
    LuaUtils::PushPoint(l, center);
    lua_pushnumber(l, radius);
    LuaUtils::PushColorArray(l, color, len);
    LuaUtils::PushScalarArray(l, pos, len);
    lua_pushstring(l, nullptr == mode ? "" : mode);

    auto count = LuaUtils::SafePCall(l, 5);
    auto shader = lua_touserdata(l, -1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
    return shader;
}

void ShaderProxy::release(BonesShader shader)
{
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodRelease);
    lua_pushlightuserdata(l, shader);
    auto count = LuaUtils::SafePCall(l, 1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
}

}