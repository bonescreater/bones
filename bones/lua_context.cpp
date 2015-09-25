#include "lua_context.h"
#include "bones_internal.h"
#include "lua_check.h"
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
static const char * kMethodGetPixmapSize = "getPixmapSize";
static const char * kMethodExsubetPixmap = "extractSubsetPixmap";

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
        auto ob = static_cast<BonesObject *>(LuaContext::CallGetCObject(l));
        bo = GetCoreInstance()->getObject(ob, lua_tostring(l, 2));
    }
    if (bo)
    {
        LuaContext::GetLOFromCO(l, bo);
        assert(lua_istable(l, -1));
    }
    return 1;
}

//资源接口 获取指定资源的宽高
static int BonesGetPixmapSize(lua_State * l)
{//(key)
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushnil(l);

    auto key = lua_tostring(l, 1);
    auto pm = GetCoreInstance()->createPixmap();
    GetCoreInstance()->getPixmap(key, *pm);
    lua_pushinteger(l, pm->getWidth());
    lua_pushinteger(l, pm->getHeight());
    GetCoreInstance()->destroyPixmap(pm);
    return 2;
}

static int BonesExtractSubsetPixmap(lua_State * l)
{//(dkey, skey, l, t, r, b)
    lua_settop(l, 6);

    auto dkey = lua_tostring(l, 1);
    auto dpm = GetCoreInstance()->createPixmap();
    auto skey = lua_tostring(l, 2);
    auto spm = GetCoreInstance()->createPixmap();
    GetCoreInstance()->getPixmap(skey, *spm);
    BonesRect sub;
    sub.left = static_cast<BonesScalar>(lua_tointeger(l, 3));
    sub.top = static_cast<BonesScalar>(lua_tointeger(l, 4));
    sub.right = static_cast<BonesScalar>(lua_tointeger(l, 5));
    sub.bottom = static_cast<BonesScalar>(lua_tointeger(l, 6));
    spm->extractSubset(*dpm, sub);
    GetCoreInstance()->clonePixmap(dkey, *dpm);
    GetCoreInstance()->destroyPixmap(spm);
    GetCoreInstance()->destroyPixmap(dpm);
    return 0;
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

        lua_pushcfunction(state, &BonesGetPixmapSize);
        lua_setfield(state, -2, kMethodGetPixmapSize);

        lua_setglobal(state, kBonesTable);
    }

    return state != nullptr;
}

void LuaContext::ShutDown()
{
    //应该close前 清空table
    Reset();  
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
        BLG_ERROR << kBonesTable << "push failed\n";
}

void LuaContext::GetCO2LOTable(lua_State * l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    GetGlobalTable(l);
    lua_getfield(l, -1, kCO2LOTable);
    if (!lua_istable(l, -1))
        BLG_ERROR << kCO2LOTable << "push failed";
    lua_copy(l, -1, -3);
    lua_pop(l, 2);
}

void LuaContext::ClearLOFromCO(lua_State * l, void * co)
{
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetCO2LOTable(l);
    lua_pushlightuserdata(l, co);
    lua_pushnil(l);
    lua_settable(l, -3);
    lua_pop(l, 1);
}

void * LuaContext::GetEventCache(lua_State * l, int count)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    lua_pushnil(l);
    GetGlobalTable(l);
    lua_getfield(l, -1, kCacheEvent);
    if (!lua_istable(l, -1))
        BLG_ERROR << kCacheEvent << "push failed";

    //得到cache table
    lua_pushinteger(l, count);
    lua_gettable(l, -2);
    if (!lua_isuserdata(l, -1))
    {
        lua_pop(l, 1);
        lua_pushinteger(l, count);
        lua_newuserdata(l, sizeof(void *));
        assert(lua_istable(l, -3));
        lua_settable(l, -3);
        lua_pushinteger(l, count);
        lua_gettable(l, -2);
    }
    lua_copy(l, -1, -4);
    lua_pop(l, 3);

    return lua_touserdata(l, -1);
}

int LuaContext::SafeLOPCall(lua_State * l, int nargs, int nresults)
{
    int top = lua_gettop(l) - nargs - 1;

    if (lua_isfunction(l, -1 - nargs))
    {
        if (lua_pcall(l, nargs, nresults, 0))
        {
            BLG_ERROR << lua_tostring(l, -1) << "\n";
            lua_pop(l, 1);
        }
    }
    else
    {
        if (!lua_isnil(l, -1 - nargs))
            BLG_VERBOSE << "LO Function ???\n";
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
        BLG_ERROR << co << "::GetLOFromCO failed\n";
}

static int GetCObject(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushlightuserdata(l, lua_touserdata(l, lua_upvalueindex(1)));

    return 1;
}

void LuaContext::SetGetCObject(lua_State * l, void * co)
{
    if (lua_istable(l, -1))
    {
        lua_pushstring(l, kMethodGetCObject);
        lua_pushlightuserdata(l, co);
        lua_pushcclosure(l, &GetCObject, 1);
        lua_settable(l, -3);
    }
}

void * LuaContext::CallGetCObject(lua_State *l)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, -1);
    assert(lua_istable(l, -1));
    lua_getfield(l, -1, kMethodGetCObject);
    assert(lua_isfunction(l, -1));
    if (!lua_isfunction(l, -1))
        return nullptr;
    lua_pushnil(l);
    lua_copy(l, -3, -1);
    auto ret = LuaContext::SafeLOPCall(l, 1, 1);
    assert(1 == ret);
    void * ud = lua_touserdata(l, -1);
    lua_pop(l, ret + 1);
    assert(ud);
    return ud;
}

}