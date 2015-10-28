#include "lua_context.h"
#include "bones.h"
#include "lua_check.h"
#include "script_parser.h"
#include "utils.h"

#include "core/logging.h"
#include "core/ref.h"
#include "core/encoding.h"

namespace bones
{

static const char * kBonesTable = "bones";
static const char * kCO2LOTable = "__co2lo";
static const char * kCacheEvent = "__event__cache";
static const char * kCacheOnPrepare = "__onPrepare";

static const char * kResourceTable = "resource";
static const char * kPathTable = "path";
static const char * kShaderTable = "shader";
static const char * kPixmapTable = "pixmap";

//static const char * kMethodAddPixmap = "addPixmap";
//static const char * kMethodAddCursor = "addCursor";
static const char * kMethodGetPixmap = "getPixmap";
static const char * kMethodGetCursor = "getCursor";
//static const char * kMethodClean = "clean";

static const char * kMethodCreate = "create";
static const char * kMethodMoveTo = "moveTo";
static const char * kMethodLineTo = "lineTo";
static const char * kMethodQuadTo = "quadTo";
static const char * kMethodConicTo = "conicTo";
static const char * kMethodCubicTo = "cubicTo";
static const char * kMethodArcTo = "arcTo";
static const char * kMethodClose = "close";

static const char * kMethodCreateLinearGradient = "createLinearGradient";
static const char * kMethodCreateRadialGradient = "createRadialGradient";

//static const char * kMethodAlloc = "alloc";
//static const char * kMethodDecode = "decode";
static const char * kMethodGetSize = "getSize";
//static const char * kMethodLock = "lock";
//static const char * kMethodUnlock = "unlock";
static const char * kMethodErase = "erase";
static const char * kMethodExtractSubset = "extractSubset";
static const char * kMethodIsTransparent = "isTransparent";

static const char * kMethodRelease = "release";
static const char * kMethodGetObject = "getObject";
static const char * kMethodCleanObject = "cleanObject";
static const char * kMethodCreateObject = "createObject";


static lua_State * state = nullptr;

class LuaObjectListener : public BonesObjectListener
{
public:
    void onPrepare(BonesObject * ob)
    {
        auto l = LuaContext::State();
        LUA_STACK_AUTO_CHECK(l);
        lua_getglobal(l, kBonesTable);
        lua_getfield(l, -1, kCacheOnPrepare);
        LuaContext::GetLOFromCO(l, ob);
        LuaContext::SafeLOPCall(l, 1, 0);
        lua_pop(l, 1);
    }
};

static LuaObjectListener lo;
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
//(parent, label, id, class_name, group, onPrepare)
//(parent, label, onPrepare)
static int BonesCreateObject(lua_State * l)
{
    lua_settop(l, 6);
    lua_pushnil(l);
    BonesObject * parent = nullptr;
    int idx = 1;
    if (lua_istable(l, 1))
    {
        idx = 2;
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        parent = static_cast<BonesObject *>(LuaContext::CallGetCObject(l));
    }
    const char * label = lua_tostring(l, idx++);
    const char * id = nullptr;
    if (lua_isstring(l, idx))
        id = lua_tostring(l, idx++);
    const char * class_name = nullptr;
    if (lua_isstring(l, idx))
        class_name = lua_tostring(l, idx++);
    const char * group_id = nullptr;
    if (lua_isstring(l, idx))
        group_id = lua_tostring(l, idx++);

    //onPrepare 放到bones中去
    assert(lua_isnil(l, idx) || lua_isfunction(l, idx));
    lua_getglobal(l, kBonesTable);
    lua_pushnil(l);
    lua_copy(l, idx, -1);
    lua_setfield(l, -2, kCacheOnPrepare);
    lua_pop(l, 1);

    auto bo = BonesGetCore()->createObject(parent, label, id, class_name, group_id, &lo);
    LuaContext::GetLOFromCO(l, bo);
    return 1;
}

static int BonesCleanObject(lua_State * l)
{
    lua_settop(l, 1);
    if (lua_istable(l, 1))
    {
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        auto ob = static_cast<BonesObject *>(LuaContext::CallGetCObject(l));
        if (ob)
            BonesGetCore()->cleanObject(ob);
    }
    return 0;
}


static int ShaderCreateLinearGradient(lua_State * l)
{//(begin.x, begin.y, end.x, end.y, mode, count, color..., pos...)
    auto count = lua_gettop(l);
    if (count > 6)
    {
        BonesPoint begin = {
            static_cast<BonesScalar>(lua_tonumber(l, 1)),
            static_cast<BonesScalar>(lua_tonumber(l, 2)) };
        BonesPoint end = {
            static_cast<BonesScalar>(lua_tonumber(l, 3)),
            static_cast<BonesScalar>(lua_tonumber(l, 4)) };
        BonesShaderProxy::TileMode mode =
            static_cast<BonesShaderProxy::TileMode>(lua_tointeger(l, 5));
        auto array_count = static_cast<size_t>(lua_tointeger(l, 6));
        count = 6 + array_count * 2;
        lua_settop(l, count);
        std::vector<BonesColor> color;
        std::vector<BonesScalar> pos;       
        for (size_t i = 0; i < array_count; ++i)
        {
            color.push_back(static_cast<BonesColor>(lua_tointeger(l, 7 + i)));
            pos.push_back(static_cast<BonesScalar>(
                lua_tonumber(l, 7 + i + array_count)));
        }
        lua_pushlightuserdata(l,
            GetCoreInstance()->getShaderProxy()->createLinearGradient(
            begin, end, mode, array_count,
            &color[0], &pos[0]));
    }
    else
        lua_pushnil(l);
    return 1;
}

static int ShaderCreateRadialGradient(lua_State * l)
{
    auto count = lua_gettop(l);
    if (count > 5)
    {
        BonesPoint center = {
            static_cast<BonesScalar>(lua_tonumber(l, 1)),
            static_cast<BonesScalar>(lua_tonumber(l, 2)) };
        BonesScalar radius = static_cast<BonesScalar>(lua_tonumber(l, 3));
        BonesShaderProxy::TileMode mode =
            static_cast<BonesShaderProxy::TileMode>(lua_tointeger(l, 4));
        auto array_count = static_cast<size_t>(lua_tointeger(l, 5));
        count = 5 + array_count * 2;
        lua_settop(l, count);
        std::vector<BonesColor> color;
        std::vector<BonesScalar> pos;
        for (size_t i = 0; i < array_count; ++i)
        {
            color.push_back(static_cast<BonesColor>(lua_tointeger(l, 6 + i)));
            pos.push_back(static_cast<BonesScalar>(
                lua_tonumber(l, 6 + i + array_count)));
        }
        lua_pushlightuserdata(l,
            GetCoreInstance()->getShaderProxy()->createRadialGradient(
            center, radius, mode, array_count,
            &color[0], &pos[0]));
    }
    else
        lua_pushnil(l);
    return 1;
}

static int ShaderRelease(lua_State * l)
{//shader
    lua_settop(l, 1);
    BonesGetCore()->getShaderProxy()->release(lua_touserdata(l, 1));
    return 0;
}
//key, pixmap
//static int ResourceAddPixmap(lua_State * l)
//{
//    if (lua_gettop(l) == 2)
//    {
//        BonesGetCore()->getResourceManager()->addPixmap(
//            lua_tostring(l, 2), lua_touserdata(l, 3));
//    }
//    else
//        BLG_ERROR << "resource." << kMethodAddPixmap << " error";
//    return 0;
//}
//key
static int ResourceGetPixmap(lua_State * l)
{
    if (lua_gettop(l) == 1)
    {
        auto pm = BonesGetCore()->getResourceManager()->getPixmap(
            lua_tostring(l, 1));
        lua_pushlightuserdata(l, pm);
    }
    else
    {
        BLG_ERROR << "resource." << kMethodGetPixmap << " error";
        lua_pushnil(l);
    }
    return 1;
}

//static int ResourceClean(lua_State * l)
//{
//    BonesGetCore()->getResourceManager()->clean();
//    return 0;
//}

static int PathCreate(lua_State * l)
{
    lua_pushlightuserdata(l, BonesGetCore()->getPathProxy()->create());
    return 1;
}
//path, x, y
static int PathMoveTo(lua_State * l)
{
    if (lua_gettop(l) != 3)
    {
        BonesPoint bp =
        {
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)),
        };
        BonesGetCore()->getPathProxy()->moveTo(
            lua_touserdata(l, 1), bp);
    }
    else
        BLG_ERROR << "path." << kMethodMoveTo << " error";
    return 0;
}
//path, x, y
static int PathLineTo(lua_State * l)
{
    if (lua_gettop(l) != 3)
    {
        BonesPoint bp =
        {
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)),
        };
        BonesGetCore()->getPathProxy()->lineTo(
            lua_touserdata(l, 1), bp);
    }
    else
        BLG_ERROR << "path." << kMethodLineTo << " error";
    return 0;
}

static int PathQuadTo(lua_State * l)
{
    if (lua_gettop(l) != 5)
    {
        BonesPoint bp[2] =
        { 
            { Utils::ToBonesScalar(lua_tonumber(l, 2)) ,
              Utils::ToBonesScalar(lua_tonumber(l, 3)), },
              { Utils::ToBonesScalar(lua_tonumber(l, 4)),
              Utils::ToBonesScalar(lua_tonumber(l, 5)), },
        };
        BonesGetCore()->getPathProxy()->quadTo(
            lua_touserdata(l, 1), bp[0], bp[1]);
    }
    else
        BLG_ERROR << "path." << kMethodQuadTo << " error";
    return 0;
}

static int PathConicTo(lua_State * l)
{
    if (lua_gettop(l) != 6)
    {
        BonesPoint bp[2] =
        {
            { Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)), },
            { Utils::ToBonesScalar(lua_tonumber(l, 4)),
            Utils::ToBonesScalar(lua_tonumber(l, 5)), },
        };
        BonesGetCore()->getPathProxy()->conicTo(
            lua_touserdata(l, 1), bp[0], bp[1],
            Utils::ToBonesScalar(lua_tonumber(l, 6)));
    }
    else
        BLG_ERROR << "path." << kMethodConicTo << " error";
    return 0;
}

static int PathCubicTo(lua_State * l)
{
    if (lua_gettop(l) != 7)
    {
        BonesPoint bp[3] =
        {
            { Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)), },
            { Utils::ToBonesScalar(lua_tonumber(l, 4)),
            Utils::ToBonesScalar(lua_tonumber(l, 5)), },
            { Utils::ToBonesScalar(lua_tonumber(l, 6)),
            Utils::ToBonesScalar(lua_tonumber(l, 7)), },
        };
        BonesGetCore()->getPathProxy()->cubicTo(
            lua_touserdata(l, 1), bp[0], bp[1], bp[2]);
    }
    else
        BLG_ERROR << "path." << kMethodCubicTo << " error";
    return 0;
}

static int PathArcTo(lua_State * l)
{
    if (lua_gettop(l) != 7)
    {
        BonesRect oval =
        {
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)),
            Utils::ToBonesScalar(lua_tonumber(l, 4)),
            Utils::ToBonesScalar(lua_tonumber(l, 5)),
        };

        BonesGetCore()->getPathProxy()->arcTo(
            lua_touserdata(l, 1), oval, 
            Utils::ToBonesScalar(lua_tonumber(l, 6)), 
            Utils::ToBonesScalar(lua_tonumber(l, 7)));
    }
    else
        BLG_ERROR << "path." << kMethodArcTo << " error";
    return 0;
}

static int PathClose(lua_State * l)
{
    if (lua_gettop(l) != 1)
        BonesGetCore()->getPathProxy()->close(lua_touserdata(l, 1));
    return 0;
}

static int PathRelease(lua_State * l)
{
    if (lua_gettop(l) != 1)
        BonesGetCore()->getPathProxy()->release(lua_touserdata(l, 1));
    return 0;
}

//static int PixmapCreate(lua_State * l)
//{
//    lua_pushlightuserdata(l, BonesGetCore()->getPixmapProxy()->create());
//    return 1;
//}
//pixmap
static int PixmapGetSize(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushnil(l);
    auto pm = lua_touserdata(l, 1);
    if (pm)
    {
        lua_pushinteger(l, BonesGetCore()->getPixmapProxy()->getWidth(pm));
        lua_pushinteger(l, BonesGetCore()->getPixmapProxy()->getHeight(pm));
    }
    return 2;
}
//self color
static int PixmapErase(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushnil(l);

    BonesGetCore()->getPixmapProxy()->erase(
        lua_touserdata(l, 1),
        Utils::ToBonesColor(lua_tointeger(l, 2)));
    return 0;
}

static int PixmapExtractSubset(lua_State * l)
{
    lua_settop(l, 6);

    BonesRect subset =
    {
        Utils::ToBonesScalar(lua_tonumber(l, 3)),
        Utils::ToBonesScalar(lua_tonumber(l, 4)),
        Utils::ToBonesScalar(lua_tonumber(l, 5)),
        Utils::ToBonesScalar(lua_tonumber(l, 6)),
    };
    BonesGetCore()->getPixmapProxy()->extractSubset(
        lua_touserdata(l, 1),
        lua_touserdata(l, 2),
        subset);
    return 0;
}
//self, x, y
static int PixmapIsTransparent(lua_State * l)
{
    lua_settop(l, 3);

    lua_pushboolean(l, 
        BonesGetCore()->getPixmapProxy()->isTransparent(
        lua_touserdata(l, 1),
        Utils::ToInt(lua_tointeger(l, 2)),
        Utils::ToInt(lua_tointeger(l, 3))));
    return 1;
}

//static int PixmapRelease(lua_State * l)
//{
//    if (lua_gettop(l) != 1)
//        BonesGetCore()->getPixmapProxy()->release(lua_touserdata(l, 1));
//    return 0;
//}

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
        //resource
        lua_newtable(state);
        //lua_pushcfunction(state, &ResourceAddPixmap);
        //lua_setfield(state, -2, kMethodAddPixmap);
        //lua_pushcfunction(state, &ResourceAddCursor);
        //lua_setfield(state, -2, kMethodAddCursor);
        lua_pushcfunction(state, &ResourceGetPixmap);
        lua_setfield(state, -2, kMethodGetPixmap);
        //lua_pushcfunction(state, &ResourceGetCursor);
        //lua_setfield(state, -2, kMethodGetCursor);
        //lua_pushcfunction(state, &ResourceClean);
        //lua_setfield(state, -2, kMethodClean);
        lua_setfield(state, -2, kResourceTable);
        //path
        lua_newtable(state);
        lua_pushcfunction(state, &PathCreate);
        lua_setfield(state, -2, kMethodCreate);
        lua_pushcfunction(state, &PathMoveTo);
        lua_setfield(state, -2, kMethodMoveTo);
        lua_pushcfunction(state, &PathLineTo);
        lua_setfield(state, -2, kMethodLineTo);
        lua_pushcfunction(state, &PathQuadTo);
        lua_setfield(state, -2, kMethodQuadTo);
        lua_pushcfunction(state, &PathConicTo);
        lua_setfield(state, -2, kMethodConicTo);
        lua_pushcfunction(state, &PathCubicTo);
        lua_setfield(state, -2, kMethodCubicTo);
        lua_pushcfunction(state, &PathArcTo);
        lua_setfield(state, -2, kMethodArcTo);
        lua_pushcfunction(state, &PathClose);
        lua_setfield(state, -2, kMethodClose);
        lua_pushcfunction(state, &PathRelease);
        lua_setfield(state, -2, kMethodRelease);
        lua_setfield(state, -2, kPathTable);
        //shader
        lua_newtable(state);
        lua_pushcfunction(state, &ShaderCreateLinearGradient);
        lua_setfield(state, -2, kMethodCreateLinearGradient);
        lua_pushcfunction(state, &ShaderCreateRadialGradient);
        lua_setfield(state, -2, kMethodCreateRadialGradient);
        lua_pushcfunction(state, &ShaderRelease);
        lua_setfield(state, -2, kMethodRelease);
        lua_setfield(state, -2, kShaderTable);
        //pixmap
        lua_newtable(state);
        //lua_pushcfunction(state, &PixmapCreate);
        //lua_setfield(state, -2, kMethodCreate);
        //lua_pushcfunction(state, &PixmapAlloc);
        //lua_setfield(state, -2, kMethodAlloc);
        //lua_pushcfunction(state, &PixmapDecode);
        //lua_setfield(state, -2, kMethodDecode);
        lua_pushcfunction(state, &PixmapGetSize);
        lua_setfield(state, -2, kMethodGetSize);
        //lua_pushcfunction(state, &PixmapLock);
        //lua_setfield(state, -2, kMethodLock);
        //lua_pushcfunction(state, &PixmapUnlock);
        //lua_setfield(state, -2, kMethodUnlock);
        lua_pushcfunction(state, &PixmapErase);
        lua_setfield(state, -2, kMethodErase);
        lua_pushcfunction(state, &PixmapExtractSubset);
        lua_setfield(state, -2, kMethodExtractSubset);
        lua_pushcfunction(state, &PixmapIsTransparent);
        lua_setfield(state, -2, kMethodIsTransparent);
        //lua_pushcfunction(state, &PixmapRelease);
        //lua_setfield(state, -2, kMethodRelease);
        lua_setfield(state, -2, kPixmapTable);

        lua_pushcfunction(state, &BonesGetObject);
        lua_setfield(state, -2, kMethodGetObject);

        lua_pushcfunction(state, &BonesCreateObject);
        lua_setfield(state, -2, kMethodCreateObject);

        lua_pushcfunction(state, &BonesCleanObject);
        lua_setfield(state, -2, kMethodCleanObject);
        //lua_pushcfunction(state, &BonesGetPixmapSize);
        //lua_setfield(state, -2, kMethodGetPixmapSize);
        
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
            BLG_ERROR << lua_tostring(l, -1);
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
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        BLG_ERROR << "CallGetCObject::unknown lua object";
        return nullptr;
    }

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

void LuaContext::PushScriptArg(lua_State * l, BonesObject::ScriptArg arg)
{
    LUA_STACK_AUTO_CHECK_COUNT(l, 1);
    switch (arg.type)
    {
    case BonesObject::ScriptArg::kNill:
        lua_pushnil(l);
        break;
    case BonesObject::ScriptArg::kString:
        lua_pushstring(l, arg.str);
        break;
    case BonesObject::ScriptArg::kInterger:
        lua_pushinteger(l, arg.integer);
        break;
    case BonesObject::ScriptArg::kNumber:
        lua_pushnumber(l, arg.number);
        break;
    case BonesObject::ScriptArg::kBoolean:
        lua_pushboolean(l, arg.boolean);
        break;
    case BonesObject::ScriptArg::kUserData:
        lua_pushlightuserdata(l, arg.ud);
        break;
    default:
        assert(0);
        lua_pushnil(l);
        break;
    }
}

BonesObject::ScriptArg LuaContext::GetScriptArg(lua_State * l, int idx)
{
    LUA_STACK_AUTO_CHECK(l);
    BonesObject::ScriptArg arg;
    switch (lua_type(l, idx))
    {
    case LUA_TNIL:
        arg.type = BonesObject::ScriptArg::kNill;
        break;
    case LUA_TBOOLEAN:
        arg.boolean = !!lua_toboolean(l, idx);
        arg.type = BonesObject::ScriptArg::kBoolean;
        break;
    case LUA_TSTRING:
        arg.str = lua_tostring(l, idx);
        arg.type = BonesObject::ScriptArg::kString;
        break;
    case LUA_TNUMBER:
        arg.number = lua_tonumber(l, idx);
        arg.type = BonesObject::ScriptArg::kNumber;
        break;
    case LUA_TLIGHTUSERDATA:
    case LUA_TUSERDATA:
        arg.ud = (void *)lua_touserdata(l, idx);
        arg.type = BonesObject::ScriptArg::kUserData;
        break;
    default:
        arg.type = BonesObject::ScriptArg::kNill;
        break;
    }
    //没有LUA_TINTEGER
    if (lua_isinteger(l, idx))
    {
        arg.integer = lua_tointeger(l, idx);
        arg.type = BonesObject::ScriptArg::kInterger;
    }
    return arg;
}

}