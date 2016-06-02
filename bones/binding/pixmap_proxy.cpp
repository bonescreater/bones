#include "pixmap_proxy.h"
#include "lua_utils.h"
#include "lua_check.h"
#include "helper/logging.h"
#include <assert.h>
#include "bitmap.h"

namespace bones
{

static const char * kProxy = "PixmapProxy";
static const char * kPath = "_pixmap_";
static const char * kMethodCreate = "create";
static const char * kMethodRelease = "release";

static int Create(lua_State * l)
{
    lua_pushnil(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    auto proxy = static_cast<PixmapProxy *>(LuaUtils::GetFieldCObject(l));
    lua_pop(l, 1);
    lua_getfield(l, -1, kPath);
    lua_pushnil(l);

    BonesPixmap * bm = Bitmap::CreateLuaNewUserData(l, proxy->ctx());
    lua_pushlightuserdata(l, bm);
    lua_copy(l, -1, -3);
    lua_pop(l, 1);

    lua_settable(l, -3);
    lua_pop(l, 2);
    LuaUtils::PopContext(l);
    //return new userdata
    return 1;
}

static int Release(lua_State * l)
{
    if (lua_gettop(l) == 1)
    {//self(new_ud)
        BonesPixmap * lightud = Bitmap::Cast(l, 1);
        LuaUtils::PushContext(l);
        lua_getfield(l, -1, kProxy);
        lua_getfield(l, -1, kPath);
        lua_pushlightuserdata(l, lightud);
        lua_pushnil(l);
        lua_settable(l, -3);

        lua_pop(l, 2);
        LuaUtils::PopContext(l);
    }
    return 0;
}

PixmapProxy::PixmapProxy(EngineContext & ctx)
{
    ctx_ = &ctx;
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_newtable(l);
    LuaUtils::SetFieldCObject(l, this);

    lua_pushcfunction(l, &Create);
    lua_setfield(l, -2, kMethodCreate);
    lua_pushcfunction(l, &Release);
    lua_setfield(l, -2, kMethodRelease);

    lua_newtable(l);
    lua_setfield(l, -2, kPath);

    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

PixmapProxy::~PixmapProxy()
{
    auto l = ctx_->State();
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_pushnil(l);
    lua_setfield(l, -2, kPath);
    lua_pop(l, 1);

    lua_pushnil(l);
    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

BonesPixmap * PixmapProxy::create()
{
    auto l = ctx_->State();

    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodCreate);
    auto count = LuaUtils::SafePCall(l, 0);
    auto path = Bitmap::Cast(l, -1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
    return path;
}

void PixmapProxy::release(BonesPixmap * pixmap)
{
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodRelease);
    lua_pushnil(l);//param

    lua_pushnil(l);
    lua_copy(l, -4, -1);//copy proxy
    lua_getfield(l, -1, kPath);
    lua_pushlightuserdata(l, pixmap);
    lua_gettable(l, -2);
    lua_copy(l, -1, -4);// copy to param
    lua_pop(l, 3);

    auto count = LuaUtils::SafePCall(l, 1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
}

EngineContext & PixmapProxy::ctx()
{
    return *ctx_;
}

}