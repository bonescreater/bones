#include "lua_utils.h"
#include "lua_check.h"

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

static inline BonesScalar ScalarFromLuaStack(lua_State * l, int idx)
{
    return LuaUtils::ToScalar(lua_tonumber(l, idx));
}

static inline int IntFromLuaStack(lua_State * l, int idx)
{
    return LuaUtils::ToInt(lua_tointeger(l, idx));
}

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

template<class T, class F>
void TPushRect(lua_State * l, T r, F func)
{
    lua_newtable(l);
    func(l, r.left);
    lua_setfield(l, -2, kStrLeft);
    func(l, r.top);
    lua_setfield(l, -2, kStrTop);
    func(l, r.right);
    lua_setfield(l, -2, kStrRight);
    func(l, r.bottom);
    lua_setfield(l, -2, kStrBottom);
}

template<class T, class F>
void TGetRect(lua_State * l, int idx, T r, F func)
{
    r.left = r.top = r.right = r.bottom = 0;
    if (!lua_istable(l, idx))
        ASSERT_RETURN;

    lua_getfield(l, idx, kStrLeft);
    r.left = func(l, -1);
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrTop);
    r.top = func(l, -1);
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrRight);
    r.right = func(l, -1);
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrBottom);
    r.bottom = func(l, -1);
    lua_pop(l, 1);
}

void LuaUtils::PushRect(lua_State * l, const BonesRect & r)
{
    TPushRect(l, r, lua_pushnumber);
}

void LuaUtils::GetRect(lua_State * l, int idx, BonesRect & r)
{
    TGetRect(l, idx, r, ScalarFromLuaStack);
}

void LuaUtils::PopRect(lua_State * l, BonesRect & r)
{
    GetRect(l, -1, r);
    lua_pop(l, 1);
}

void LuaUtils::PushIRect(lua_State * l, const BonesIRect & r)
{
    TPushRect(l, r, lua_pushinteger);
}

void LuaUtils::GetIRect(lua_State * l, int idx, BonesIRect & r)
{
    TGetRect(l, idx, r, IntFromLuaStack);
}

void LuaUtils::PopIRect(lua_State * l, BonesIRect & r)
{
    GetIRect(l, -1, r);
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

template<class T, class F>
void TPushSize(lua_State * l, T se, F func)
{
    lua_newtable(l);
    func(l, se.width);
    lua_setfield(l, -2, kStrWidth);
    func(l, se.height);
    lua_setfield(l, -2, kStrHeight);
}

template<class T, class F>
void TGetSize(lua_State * l, int idx, T se, F func)
{
    se.width = se.height = 0;
    if (!lua_istable(l, idx))
        ASSERT_RETURN;
    lua_getfield(l, idx, kStrWidth);
    se.width = func(l, -1);
    lua_pop(l, 1);
    lua_getfield(l, idx, kStrHeight);
    se.height = func(l, -1);
    lua_pop(l, 1);
}

void LuaUtils::PushSize(lua_State * l, const BonesSize & se)
{
    TPushSize(l, se, lua_pushnumber);
}

void LuaUtils::GetSize(lua_State * l, int idx, BonesSize & se)
{
    TGetSize(l, idx, se, ScalarFromLuaStack);
}

void LuaUtils::PopSize(lua_State * l, BonesSize & se)
{
    GetSize(l, -1, se);
    lua_pop(l, 1);
}

void LuaUtils::PushISize(lua_State * l, const BonesISize & se)
{
    TPushSize(l, se, lua_pushinteger);
}

void LuaUtils::GetISize(lua_State * l, int idx, BonesISize & se)
{
    TGetSize(l, idx, se, IntFromLuaStack);
}

void LuaUtils::PopISize(lua_State * l, BonesISize & se)
{
    GetISize(l, -1, se);
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

static const char * kMethodGetCObject = "_getCObject_";

static int GetCObject(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushlightuserdata(l, lua_touserdata(l, lua_upvalueindex(1)));

    return 1;
}

void LuaUtils::SetFieldCObject(lua_State *l, void * co)
{
    if (lua_istable(l, -1))
    {
        lua_pushstring(l, kMethodGetCObject);
        lua_pushlightuserdata(l, co);
        lua_pushcclosure(l, &GetCObject, 1);
        lua_settable(l, -3);
    }
}

void * LuaUtils::GetFieldCObject(lua_State * l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    assert(lua_istable(l, -1));
    if (!lua_istable(l, -1))
    {
        lua_pushnil(l);
        return nullptr;
    }

    lua_getfield(l, -1, kMethodGetCObject);
    assert(lua_isfunction(l, -1));
    if (!lua_isfunction(l, -1))
        return nullptr;
    lua_pushnil(l);
    lua_copy(l, -3, -1);
    auto ret = SafePCall(l, 1);
    assert(1 == ret);
    void * ud = lua_touserdata(l, -1);
    assert(ud);
    return ud;
}

static const char * kMethodIndex = "__index";
static const char * kMethodGC = "__gc";

void LuaUtils::CreateMetaTable(lua_State * l, const char * mt, lua_CFunction gc)
{
    luaL_newmetatable(l, mt);
    lua_pushnil(l);
    lua_copy(l, -2, -1);
    lua_setfield(l, -2, kMethodIndex);
    lua_pushcfunction(l, gc);
    lua_setfield(l, -2, kMethodGC);
}

}