#include "lua_utils.h"
#include <assert.h>
#include "helper/logging.h"

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

static const char * kStrContext = "bones";

#define ASSERT_RETURN {assert(0);  return;}

void LuaUtils::PushContext(lua_State * l)
{
    lua_getglobal(l, kStrContext);
    assert(lua_istable(l, -1));
}

void LuaUtils::PopContext(lua_State * l)
{
    assert(lua_istable(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::CreateContext(lua_State * l)
{
    lua_newtable(l);
    lua_setglobal(l, kStrContext);
}

void LuaUtils::DestroyContext(lua_State * l)
{
    lua_pushnil(l);
    lua_setglobal(l, kStrContext);
}

inline BonesScalar LuaUtils::ToScalar(lua_Number t)
{
    return static_cast<BonesScalar>(t);
}

inline BonesColor LuaUtils::ToColor(lua_Integer t)
{
    return static_cast<BonesColor>(t);
}

inline int LuaUtils::ToInt(lua_Integer t)
{
    return static_cast<int>(t);
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

void LuaUtils::PushRect(lua_State * l, const BonesRect & r)
{
    lua_newtable(l);
    lua_pushnumber(l, r.left);
    lua_setfield(l, -2, kStrLeft);
    lua_pushnumber(l, r.top);
    lua_setfield(l, -2, kStrTop);
    lua_pushnumber(l, r.right);
    lua_setfield(l, -2, kStrRight);
    lua_pushnumber(l, r.bottom);
    lua_setfield(l, -2, kStrBottom);
}

void LuaUtils::GetRect(lua_State * l, int idx, BonesRect & r)
{
    r.left = r.top = r.right = r.bottom = 0;
    if (!lua_istable(l, idx))
        ASSERT_RETURN;

    lua_getfield(l, idx, kStrLeft);
    r.left = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrTop);
    r.top = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrRight);
    r.right = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrBottom);
    r.bottom = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::PopRect(lua_State * l, BonesRect & r)
{
    GetRect(l, -1, r);
    lua_pop(l, 1);
}

void LuaUtils::PushPoint(lua_State * l, const BonesPoint & pt)
{
    lua_newtable(l);
    lua_pushnumber(l, pt.x);
    lua_setfield(l, -2, kStrX);
    lua_pushnumber(l, pt.y);
    lua_setfield(l, -2, kStrY);
}

void LuaUtils::GetPoint(lua_State * l, int idx, BonesPoint & pt)
{
    pt.x = pt.y = 0;
    if (!lua_istable(l, idx))
        ASSERT_RETURN;
    lua_getfield(l, idx, kStrX);
    pt.x = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrY);
    pt.y = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::PopPoint(lua_State * l, BonesPoint & pt)
{
    GetPoint(l, -1, pt);
    lua_pop(l, 1);
}

void LuaUtils::PushSize(lua_State * l, const BonesSize & se)
{
    lua_newtable(l);
    lua_pushnumber(l, se.width);
    lua_setfield(l, -2, kStrWidth);
    lua_pushnumber(l, se.height);
    lua_setfield(l, -2, kStrHeight);
}

void LuaUtils::GetSize(lua_State * l, int idx, BonesSize & se)
{
    se.width = se.height = 0;
    if (!lua_istable(l, idx))
        ASSERT_RETURN;
    lua_getfield(l, idx, kStrWidth);
    se.width = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrHeight);
    se.height = ToScalar(lua_tonumber(l, -1));
    lua_pop(l, 1);
}

void LuaUtils::PopSize(lua_State * l, BonesSize & se)
{
    GetSize(l, -1, se);
    lua_pop(l, 1);
}

void LuaUtils::PushColorMatrix(lua_State * l, BonesScalar * cm, int len)
{
    lua_newtable(l);
    if (cm && len)
    {
        for (int i = 0; i < len; ++i)
        {
            lua_pushnumber(l, cm[i]);
            lua_seti(l, -2, i + 1);
        }
    }
}

void LuaUtils::GetColorMatrix(lua_State * l, int idx, BonesScalar * cm, int len)
{
    memset(cm, 0, sizeof(*cm) * len);
    if (!lua_istable(l, idx) || len < kColorMatrxSize)
        ASSERT_RETURN;
    for (int i = 0; i < len; ++i)
    {
        lua_geti(l, idx, i + 1);
        cm[i] = ToScalar(lua_tonumber(l, -1));
        lua_pop(l, 1);
    }
}

void LuaUtils::PopColorMatrix(lua_State * l, BonesScalar * cm, int len)
{
    GetColorMatrix(l, -1, cm, len);
    lua_pop(l, 1);
}


void LuaUtils::PushColorArray(lua_State * l, const BonesColor * colors, int len)
{
    lua_newtable(l);
    if (colors && len)
    {
        for (int i = 0; i < len; ++i)
        {
            lua_pushinteger(l, colors[i]);
            lua_seti(l, -2, i + 1);
        }
    }
}

void LuaUtils::GetColorArray(lua_State * l, int idx, BonesColor * colors, int len)
{
    if (nullptr == colors || 0 == len)
        return;
    memset(colors, 0, sizeof(*colors) * len);
    if (!lua_istable(l, idx))
        ASSERT_RETURN;
    for (int i = 0; i < len; i++)
    {
        lua_geti(l, idx, i + 1);
        colors[i] = ToColor(lua_tointeger(l, -1));
        lua_pop(l, 1);
    }
}

void LuaUtils::PopColorArray(lua_State * l, BonesColor * colors, int len)
{
    GetColorArray(l, -1, colors, len);
    lua_pop(l, 1);
}

void LuaUtils::PushScalarArray(lua_State * l, const BonesScalar * scalars, int len)
{
    lua_newtable(l);
    if (scalars && len)
    {
        for (int i = 0; i < len; ++i)
        {
            lua_pushnumber(l, scalars[i]);
            lua_seti(l, -2, i + 1);
        }
    }
}

void LuaUtils::GetScalarArray(lua_State * l, int idx, BonesScalar * scalars, int len)
{
    if (nullptr == scalars || 0 == len)
        return;
    memset(scalars, 0, sizeof(*scalars) * len);
    if (!lua_istable(l, idx))
        ASSERT_RETURN;
    for (int i = 0; i < len; i++)
    {
        lua_geti(l, idx, i + 1);
        scalars[i] = ToScalar(lua_tonumber(l, -1));
        lua_pop(l, 1);
    }
}

void LuaUtils::PopScalarArray(lua_State * l, BonesScalar * scalars, int len)
{
    GetScalarArray(l, -1, scalars, len);
    lua_pop(l, 1);
}

}