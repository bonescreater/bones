#include "resource_proxy.h"
#include "lua_check.h"
#include "lua_utils.h"
#include "bitmap.h"

namespace bones
{

static const char * kProxy = "ResourceProxy";
static const char * kMethodAdd = "add";
static const char * kMethodRemove = "remove";
static const char * kMethodClean = "clean";
static const char * kMethodGet = "get";

static inline Bitmap * Cast(void * path)
{
    if (!path)
        return nullptr;
    return static_cast<Bitmap *>(static_cast<BonesPixmap *>(*(void **)path));
}

static int Add(lua_State * l)
{
    auto count = lua_gettop(l);
    if (2 == count)
    {//key, bm
        auto key = lua_tostring(l, 1);
        auto bm = Cast(lua_touserdata(l, 2));
        if (bm)
        {
            LuaUtils::PushContext(l);
            lua_getfield(l, -1, kProxy);
            auto proxy = static_cast<ResourceProxy *>(LuaUtils::GetFieldCObject(l));
            lua_pop(l, 1);
            proxy->invokeAdd(key, *bm);
        }
    }    
    return 0;
}

static int Remove(lua_State * l)
{
    auto count = lua_gettop(l);
    if (1 == count)
    {//key
        auto key = lua_tostring(l, 1);
        LuaUtils::PushContext(l);
        lua_getfield(l, -1, kProxy);
        auto proxy = static_cast<ResourceProxy *>(LuaUtils::GetFieldCObject(l));
        lua_pop(l, 1);
        proxy->invokeRemove(key);
    }

    return 0;
}

static int Clean(lua_State * l)
{
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    auto proxy = static_cast<ResourceProxy *>(LuaUtils::GetFieldCObject(l));
    lua_pop(l, 1);
    proxy->invokeClean();

    return 0;
}

static int Get(lua_State * l)
{
    auto count = lua_gettop(l);
    bool ret = false;
    if (2 == count)
    {
        auto key = lua_tostring(l, 1);
        auto bm = Cast(lua_touserdata(l, 2));

        LuaUtils::PushContext(l);
        lua_getfield(l, -1, kProxy);
        auto proxy = static_cast<ResourceProxy *>(LuaUtils::GetFieldCObject(l));
        lua_pop(l, 1);
        ret = proxy->invokeGet(key, *bm);
    }
    return ret;
}


ResourceProxy::ResourceProxy(EngineContext & ctx)
:ctx_(&ctx)
{
    ctx_ = &ctx;
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_newtable(l);
    LuaUtils::SetFieldCObject(l, this);

    lua_pushcfunction(l, &Add);
    lua_setfield(l, -2, kMethodAdd);

    lua_pushcfunction(l, &Remove);
    lua_setfield(l, -2, kMethodRemove);

    lua_pushcfunction(l, &Clean);
    lua_setfield(l, -2, kMethodClean);

    lua_pushcfunction(l, &Get);
    lua_setfield(l, -2, kMethodGet);

    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

ResourceProxy::~ResourceProxy()
{
    auto l = ctx_->State();
    LuaUtils::PushContext(l);
    lua_pushnil(l);
    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

void ResourceProxy::add(const char * key, BonesPixmap & pm)
{
    invokeAdd(key, *static_cast<Bitmap *>(&pm));
}

void ResourceProxy::remove(const char * key)
{
    invokeRemove(key);
}

void ResourceProxy::clean() 
{
    invokeClean();
}

bool ResourceProxy::get(const char * key, BonesPixmap & pm)
{
    return invokeGet(key, *static_cast<Bitmap *>(&pm));
}

void ResourceProxy::invokeAdd(const char * key, Bitmap & pm)
{
    if (!key)
        return;

    auto iter = pixmaps_.find(key);
    if (iter != pixmaps_.end())
        *(iter->second) = pm;
    else
    {
        auto src = new Bitmap(*ctx_);
        *src = pm;
        pixmaps_[key] = src;
    }
}

void ResourceProxy::invokeRemove(const char * key)
{
    if (!key)
        return;

    auto iter = pixmaps_.find(key);
    if (iter != pixmaps_.end())
    {
        delete iter->second;
        pixmaps_.erase(iter);
    }
}

void ResourceProxy::invokeClean()
{
    for (auto iter = pixmaps_.begin(); iter != pixmaps_.end(); ++iter)
    {
        delete iter->second;
    }
    pixmaps_.clear();
}

bool ResourceProxy::invokeGet(const char * key, Bitmap & pm)
{
    if (!key)
        return false;
    auto iter = pixmaps_.find(key);
    if (iter != pixmaps_.end())
    {
        pm = *(iter->second);
        return true;
    }
    return false;
}


}