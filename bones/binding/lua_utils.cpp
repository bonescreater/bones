#include "lua_utils.h"
#include "core/logging.h"

namespace bones
{

static const char * kStrLeft = "left";
static const char * kStrTop = "top";
static const char * kStrRight = "right";
static const char * kStrBottom = "bottom";
static const char * kStrX = "x";
static const char * kStrY = "y";
static const char * kStrWidth = "width";
static const char * kStrHeight = "height";

#define ASSERT_RETURN {assert(0);  return;}

Scalar LuaUtils::ToScalar(lua_Number t)
{
    return static_cast<Scalar>(t);
}

int LuaUtils::SafePCall(lua_State * l, int nargs)
{
    int top = lua_gettop(l) - nargs - 1;

    if (lua_isfunction(l, -1 - nargs))
    {
        if (lua_pcall(l, nargs, LUA_MULTRET, 0))
        {
            BLG_ERROR << lua_tostring(l, -1);
            lua_pop(l, 1);
        }
    }
    else
    {
        if (!lua_isnil(l, -1 - nargs))
            BLG_VERBOSE << "Function ???\n";
        lua_pop(l, 1 + nargs);
    }

    return lua_gettop(l) - top;
}

void LuaUtils::PushRect(lua_State * l, Scalar left, Scalar top, Scalar right, Scalar bottom)
{
    lua_newtable(l);
    lua_pushnumber(l, left);
    lua_setfield(l, -2, kStrLeft);
    lua_pushnumber(l, top);
    lua_setfield(l, -2, kStrTop);
    lua_pushnumber(l, right);
    lua_setfield(l, -2, kStrRight);
    lua_pushnumber(l, bottom);
    lua_setfield(l, -2, kStrBottom);
}

void LuaUtils::GetRect(lua_State * l, Scalar & left, Scalar & top, Scalar & right, Scalar & bottom)
{
    left = top = right = bottom = 0;
    if (!lua_istable(l, -1))
        ASSERT_RETURN;

    lua_getfield(l, -1, kStrLeft);
    left = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, -1, kStrTop);
    top = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, -1, kStrRight);
    right = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, -1, kStrBottom);
    bottom = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::PopRect(lua_State * l, Scalar & left, Scalar & top, Scalar & right, Scalar & bottom)
{
    GetRect(l, left, top, right, bottom);
    lua_pop(l, 1);
}

void LuaUtils::PushPoint(lua_State * l, Scalar x, Scalar y)
{
    lua_newtable(l);
    lua_pushnumber(l, x);
    lua_setfield(l, -2, kStrX);
    lua_pushnumber(l, y);
    lua_setfield(l, -2, kStrY);
}

void LuaUtils::GetPoint(lua_State * l, Scalar & x, Scalar & y)
{
    x = y = 0;
    if (!lua_istable(l, -1))
        ASSERT_RETURN;
    lua_getfield(l, -1, kStrX);
    x = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, -2, kStrY);
    y = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::PopPoint(lua_State * l, Scalar & x, Scalar & y)
{
    GetPoint(l, x, y);
    lua_pop(l, 1);
}

void LuaUtils::PushSize(lua_State * l, Scalar w, Scalar h)
{
    lua_newtable(l);
    lua_pushnumber(l, w);
    lua_setfield(l, -2, kStrWidth);
    lua_pushnumber(l, h);
    lua_setfield(l, -2, kStrHeight);
}

void LuaUtils::GetSize(lua_State * l, Scalar & w, Scalar & h)
{
    w = h = 0;
    if (!lua_istable(l, -1))
        ASSERT_RETURN;
    lua_getfield(l, -1, kStrWidth);
    w = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, -1, kStrHeight);
    h = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::PopSize(lua_State * l, Scalar & w, Scalar & h)
{
    GetSize(l, w, h);
    lua_pop(l, 1);
}

void LuaUtils::PushColorMatrix(lua_State * l, Scalar * cm, size_t len)
{
    lua_newtable(l);
    for (size_t i = 0; i < len; ++i)
    {
        lua_pushinteger(l, i + 1);
        lua_pushnumber(l, cm[i]);
        lua_settable(l, -2);
    }
}

void LuaUtils::GetColorMatrix(lua_State * l, Scalar * cm, size_t len)
{
    memset(cm, 0, sizeof(*cm) * len);
    if (!lua_istable(l, -1) || len < kColorMatrxSize)
        ASSERT_RETURN;
    for (size_t i = 0; i < len; ++i)
    {
        lua_pushinteger(l, i + 1);
        lua_gettable(l, -2);
        cm[i] = ToScalar(lua_tonumber(l, -1));
        lua_pop(l, 1);
    }
}

void LuaUtils::PopColorMatrix(lua_State * l, Scalar * cm, size_t len)
{
    GetColorMatrix(l, cm, len);
    lua_pop(l, 1);
}

}