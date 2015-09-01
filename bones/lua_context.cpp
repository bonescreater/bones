#include "lua_context.h"
#include "bones_internal.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "script_parser.h"

#include "core/logging.h"
#include "core/ref.h"
#include "core/encoding.h"

namespace bones
{

static const char * kBonesTable = "bones";
static const char * kCO2LOTable = "__co2lo";
static const char * kCacheEvent = "__event__cache";

static const char * kMethodGetObject = "getObject";
static const char * kMethodDecodePixmap = "decodePixmap";
static const char * kMethodFreePixmap = "freePixmap";
static const char * kMethodLoadImage = "loadImage";

static lua_State * state = nullptr;

//(id) || (ob, id)
static int BonesGetObject(lua_State * l)
{
    auto count = lua_gettop(l);
    lua_settop(l, 2);
    lua_pushnil(l);
    BonesObject * bo = nullptr;
    if (lua_isstring(l, 1))
    {//(self, id)
        bo = GetCoreInstance()->getObject(lua_tostring(l, 1));
    }
    else if (lua_istable(l, 1))
    {//(self, ob, id)
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        auto ob = static_cast<BonesObject *>(LuaMetaTable::CallGetCObject(l));
        bo = GetCoreInstance()->getObject(ob, lua_tostring(l, 2));
    }
    if (bo)
    {
        LuaContext::GetLOFromCO(l, bo);
        assert(lua_istable(l, -1));
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
        lua_newtable(state);
        lua_setfield(state, -2, kCacheEvent);

        lua_pushcfunction(state, &BonesGetObject);
        lua_setfield(state, -2, kMethodGetObject);

        //lua_pushcfunction(state, &BonesDecodePixmap);
        //lua_setfield(state, -2, kMethodDecodePixmap);

        //lua_pushcfunction(state, &BonesFreePixmap);
        //lua_setfield(state, -2, kMethodFreePixmap);

        //lua_pushcfunction(state, &BonesLoadImage);
        //lua_setfield(state, -2, kMethodLoadImage);
        lua_setglobal(state, kBonesTable);
    }

    return state != nullptr;
}

void LuaContext::ShutDown()
{
    lua_gc(state, LUA_GCCOLLECT, 0);
    //应该close前 清空table
    lua_pushnil(state);
    lua_setglobal(state, kBonesTable);
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
    lua_gc(state, LUA_GCCOLLECT, 0);
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

void * LuaContext::GetEventCache(lua_State * l, int count)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    GetGlobalTable(l);
    lua_getfield(l, -1, kCacheEvent);
    if (!lua_istable(l, -1))
        LOG_ERROR << kCacheEvent << "push failed";
    lua_copy(l, -1, -3);
    lua_pop(l, 2);
    //得到cache table
    lua_pushinteger(l, count);
    lua_gettable(l, -2);
    if (!lua_isuserdata(l, -1))
    {
        lua_pop(l, -1);
        lua_pushinteger(l, count);
        lua_newuserdata(l, sizeof(void *));
        lua_settable(l, -3);
        lua_pushinteger(l, count);
        lua_gettable(l, -2);
    }

    return lua_touserdata(l, -1);
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

void LuaContext::GetLOFromCO(lua_State * l, BonesObject * co)
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

void LuaContext::GetLOFromCO(lua_State * l, BonesAnimation * ani)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, ani);
    lua_gettable(l, -2);
    lua_copy(l, -1, -3);
    lua_pop(l, 2);
    if (!lua_istable(l, -1))
        LOG_ERROR << ani << "::GetLOFromCO failed\n";
}

}