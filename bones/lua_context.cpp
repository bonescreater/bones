#include "lua_context.h"
#include "lua_check.h"
#include "bones_export.h"
#include "core/logging.h"
#include "core/ref.h"
#include "core/encoding.h"

namespace bones
{

static const char * kBonesTable = "bones";
static const char * kCO2LOTable = "__co2lo__";
static const char * kCacheEvent = "__event__cache__";

static const char * kMethodGetCObjectByID = "getCObjectByID";
static const char * kMethodGetLuaObjectByID = "getLuaObjectByID";
static const char * kMethodDecodePixmap = "decodePixmap";
static const char * kMethodFreePixmap = "freePixmap";
static const char * kMethodLoadImage = "loadImage";

static lua_State * state = nullptr;
//(id)
static int BonesGetCObjectByID(lua_State * l)
{   
    auto count = lua_gettop(l);  
    lua_pushnil(l);
    if (1 == count)
    {
        auto id = lua_tostring(l, 1);
        auto co = ::BonesGetCObjectByID(id);
        lua_pushlightuserdata(l, co);
    }
    return 1;
}

static int BonesGetLuaObjectByID(lua_State * l)
{
    auto count = lua_gettop(l);
    lua_pushnil(l);
    if (1 == count)
    {
        LuaContext::GetGlobalTable(l);
        lua_getfield(l, -1, kMethodGetCObjectByID);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        auto count = LuaContext::SafeLOPCall(l, 1, 1);
        assert(lua_isuserdata(l, -1));
        auto co = lua_touserdata(l, -1);
        LuaContext::GetCO2LOTable(l);
        lua_pushnil(l);
        lua_copy(l, -3, -1);
        lua_gettable(l, -2);
        assert(lua_istable(l, -1));
    }
    return 1;
}
//(data, len)
static int BonesDecodePixmap(lua_State * l)
{
    auto count = lua_gettop(l);
    lua_pushnil(l);
    if (2 == count)
    {
        auto data = lua_tostring(l, 1);
        auto len = (int)lua_tointeger(l, 2);
        auto pm = ::BonesDecodePixmap(data, len);
        //lua_pushlightuserdata(l, pm);
        //user data 在lua 代码中无法转为数字 所以只能使用数字替代
        lua_pushinteger(l, (lua_Integer)pm);
    }
    return 1;
}
//(pixmap *)
static int BonesFreePixmap(lua_State * l)
{
    auto count = lua_gettop(l);
    if (1 == count)
    {
        auto pm = lua_tointeger(l, 1);
        ::BonesFreePixmap((Pixmap *)(pm));
    }
    return 0;
}

//equal ::LoadImage
static int BonesLoadImage(lua_State * l)
{
    auto count = lua_gettop(l);
    lua_pushnil(l);
    if (6 == count)
    {
        auto hinst = ::lua_touserdata(l, 1);
        const wchar_t * n = nullptr;
        std::wstring wname;
        if (lua_isinteger(l, 2))
            n = MAKEINTRESOURCE(lua_tointeger(l, 2));
        else
        {
            auto name = lua_tostring(l, 2);
            wname = Encoding::FromUTF8(name);
            n = wname.data();
        }
        auto type = lua_tointeger(l, 3);
        auto cx = lua_tointeger(l, 4);
        auto cy = lua_tointeger(l, 5);
        auto fuload = lua_tointeger(l, 6);
        
        auto h = ::LoadImage((HINSTANCE)hinst, n, (UINT)type, (int)cx, (int)cy, (UINT)fuload);
        lua_pushinteger(l, (lua_Integer)h);
    }
    return 1;
}

bool LuaContext::StartUp()
{
    state = luaL_newstate();
    if (state)
    {
        luaL_openlibs(state);
        LUA_STACK_AUTO_CHECK(state);

        lua_newtable(state);
        lua_newtable(state);
        lua_setfield(state, -2, kCO2LOTable);
        lua_newuserdata(state, sizeof(void *));
        lua_setfield(state, -2, kCacheEvent);
        
        lua_pushcfunction(state, &BonesGetCObjectByID);
        lua_setfield(state, -2, kMethodGetCObjectByID);

        lua_pushcfunction(state, &BonesGetLuaObjectByID);
        lua_setfield(state, -2, kMethodGetLuaObjectByID);

        lua_pushcfunction(state, &BonesDecodePixmap);
        lua_setfield(state, -2, kMethodDecodePixmap);

        lua_pushcfunction(state, &BonesFreePixmap);
        lua_setfield(state, -2, kMethodFreePixmap);

        lua_pushcfunction(state, &BonesLoadImage);
        lua_setfield(state, -2, kMethodLoadImage);
        lua_setglobal(state, kBonesTable);
    }

    return state != nullptr;
}

void LuaContext::ShutDown()
{
    //lua_gc(state, LUA_GCCOLLECT, 0);
    if (state)
        lua_close(state);
    state = nullptr;
}

void LuaContext::Reset()
{
    GetGlobalTable(state);
    lua_pushnil(state);
    lua_setfield(state, -2, kCO2LOTable);
    lua_newtable(state);
    lua_setfield(state, -2, kCO2LOTable);
    lua_pop(state, 1);
}

lua_State * LuaContext::State()
{
    return state;
}

void LuaContext::GetGlobalTable(lua_State * l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_getglobal(l, kBonesTable);
    if (!lua_istable(l, -1))
        LOG_ERROR << kBonesTable << "push failed\n";
}

void LuaContext::GetCO2LOTable(lua_State * l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    GetGlobalTable(l);
    lua_getfield(l, -1, kCO2LOTable);
    if (!lua_istable(l, -1))
        LOG_ERROR << kCO2LOTable << "push failed";
    lua_copy(l, -1, -3);
    lua_pop(l, 2);
}

void LuaContext::GetEventCache(lua_State * l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    GetGlobalTable(l);
    lua_getfield(l, -1, kCacheEvent);
    lua_copy(l, -1, -3);
    lua_pop(l, 2);
    if (!lua_isuserdata(l, -1))
        LOG_ERROR << "CacheEvent Error\n";
}

int LuaContext::SafeLOPCall(lua_State * l, int nargs, int nresults)
{
    int top = lua_gettop(l) - nargs - 1;

    if (lua_isfunction(l, -1 - nargs))
    {
        if (lua_pcall(l, nargs, nresults, 0))
        {
            LOG_ERROR << lua_tostring(l, -1) << "\n";
            lua_pop(l, 1);
        }
    }
    else
    {
        if (!lua_isnil(l, -1 - nargs))
            LOG_VERBOSE << "LO Function ???\n";
        lua_pop(l, 1 + nargs);
    }

    return lua_gettop(l) - top;
}

void LuaContext::GetLOFromCO(lua_State * l, void * co)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_gettable(l, -2);
    lua_copy(l, -1, -3);
    lua_pop(l, 2);
    if (!lua_istable(l, -1))
        LOG_ERROR << co << "::GetLOFromCO failed\n";
}


}