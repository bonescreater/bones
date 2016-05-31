#include "pixmap_proxy.h"
#include "lua_utils.h"
#include "lua_check.h"
#include "helper/logging.h"
#include "SkBitmap.h"
#include <assert.h>
#include "core/color.h"
#include "SkRect.h"
#include "SkImageDecoder.h"

namespace bones
{

static const char * kProxy = "PixmapProxy";
static const char * kPath = "_pixmap_";
static const char * kMetaTable = "_mt_pixmap_";

static const char * kMethodCreate = "create";
static const char * kMethodRelease = "release";
static const char * kMethodAlloc = "alloc";
static const char * kMethodDecode = "decode";
static const char * kMethodGetWidth = "getWidth";
static const char * kMethodGetHeight = "getHeight";
static const char * kMethodLock = "lock";
static const char * kMethodUnlock = "unlock";
static const char * kMethodErase = "erase";
static const char * kMethodExtractSubset = "extractSubset";
static const char * kMethodIsTransparent = "isTransparent";

class Bitmap : public BonesPixmap
{
public:
    Bitmap(EngineContext & ctx);

    virtual ~Bitmap();

    bool alloc(const BonesISize & size) override;

    bool decode(const void * data, size_t len) override;

    int getWidth() const override;

    int getHeight() const override;

    void lock() override;

    void unlock() override;

    void erase(BonesColor color) override;

    bool extractSubset(BonesPixmap & dst, const BonesIRect & subset) override;

    bool isTransparent(int x, int y) override;

public:
    bool invokeAlloc(const BonesISize & size);

    bool invokeDecode(const void * data, size_t len);

    int invokeGetWidth() const;

    int invokeGetHeight() const;

    void invokeLock();

    void invokeUnlock();

    void invokeErase(BonesColor color);

    bool invokeExtractSubset(BonesPixmap &dst, const BonesIRect & subset);

    bool invokeIsTransparent(int x, int y);

    SkBitmap & GetBitmap();
private:
    EngineContext * ctx_;
    SkBitmap bitmap_;
};

static inline Bitmap * Cast(void * path)
{
    return static_cast<Bitmap *>(static_cast<BonesPixmap *>(*(void **)path));
}

static int Alloc(lua_State * l)
{
    auto count = lua_gettop(l);
    bool ret = false;
    if (3 == count)
    {//self, w, h
        BonesISize size = { 
            LuaUtils::ToInt(lua_tointeger(l, 2)),
            LuaUtils::ToInt(lua_tointeger(l, 3)) };
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            ret = bm->invokeAlloc(size);            
    }
    else if (2 == count)
    {//self, table
        BonesISize size;
        LuaUtils::GetISize(l, 2, size);
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            ret = bm->invokeAlloc(size);
    }
    
    lua_pushboolean(l, ret);

    return 1;
}

static int Decode(lua_State * l)
{
    auto count = lua_gettop(l);
    bool ret = false;
    if (3 == count)
    {//self, data, len
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            ret = bm->invokeDecode(
            lua_tostring(l, 2),
            static_cast<size_t>(LuaUtils::ToInt(lua_tointeger(l, 3))));
    }
    else if (2 == count)
    {//self, lstr
        size_t len = 0;
        auto data = lua_tolstring(l, 2, &len);
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            ret = bm->invokeDecode(data, len);
    }
    lua_pushboolean(l, ret);
    return 1;

}

static int GetWidth(lua_State * l)
{
    int value = 0;
    if (1 == lua_gettop(l))
    {//self
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            value = bm->invokeGetWidth();
    }
    lua_pushinteger(l, value);
    return 1;
}

static int GetHeight(lua_State * l)
{
    int value = 0;
    if (1 == lua_gettop(l))
    {//self
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            value = bm->invokeGetHeight();
    }
    lua_pushinteger(l, value);
    return 1;
}

static int Lock(lua_State * l)
{
    if (1 == lua_gettop(l))
    {
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            bm->lock();
    }
    return 0;
}

static int Unlock(lua_State * l)
{
    if (1 == lua_gettop(l))
    {
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            bm->invokeUnlock();
    }
    return 0;
}

static int Erase(lua_State * l)
{
    if (2 == lua_gettop(l))
    {
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            bm->invokeErase(LuaUtils::ToColor(lua_tointeger(l, 2)));
    }
    return 0;
}

static int ExtractSubset(lua_State * l)
{
    bool ret = false;
    if (3 == lua_gettop(l))
    {//self, dst, table
        auto bm = Cast(lua_touserdata(l, 1));
        auto dst = Cast(lua_touserdata(l, 2));
        BonesIRect iret;
        LuaUtils::GetIRect(l, 3, iret);
        if (bm && dst)
            ret = bm->invokeExtractSubset(*dst, iret);
    }
    else if (6 == lua_gettop(l))
    {//self, dst, left, top, right, bottom
        BonesIRect iret;
        iret.bottom = LuaUtils::ToInt(lua_tointeger(l, 6));
        iret.right = LuaUtils::ToInt(lua_tointeger(l, 5));
        iret.top = LuaUtils::ToInt(lua_tointeger(l, 4));
        iret.left = LuaUtils::ToInt(lua_tointeger(l, 3));
        auto bm = Cast(lua_touserdata(l, 1));
        auto dst = Cast(lua_touserdata(l, 2));
        if (bm && dst)
            ret = bm->invokeExtractSubset(*dst, iret);
    }

    lua_pushboolean(l, ret);
    return 1;
}

static int IsTransparent(lua_State * l)
{
    if (3 == lua_gettop(l))
    {
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            bm->invokeIsTransparent(
            LuaUtils::ToInt(lua_tointeger(l, 2)),
            LuaUtils::ToInt(lua_tointeger(l, 3)));
    }
    return 0;
}


static int GC(lua_State *l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        auto bm = Cast(lua_touserdata(l, 1));
        if (bm)
            delete bm;
    }
    return 0;
}

static int Create(lua_State * l)
{
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    auto proxy = static_cast<PixmapProxy *>(LuaUtils::GetFieldCObject(l));
    lua_pop(l, 1);
    lua_getfield(l, -1, kPath);
    luaL_getmetatable(l, kMetaTable);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        LuaUtils::CreateMetaTable(l, kMetaTable, &GC);
        lua_pushcfunction(l, &Alloc);
        lua_setfield(l, -2, kMethodAlloc);
        lua_pushcfunction(l, &Decode);
        lua_setfield(l, -2, kMethodDecode);
        lua_pushcfunction(l, &GetWidth);
        lua_setfield(l, -2, kMethodGetWidth);
        lua_pushcfunction(l, &GetHeight);
        lua_setfield(l, -2, kMethodGetHeight);
        lua_pushcfunction(l, &Lock);
        lua_setfield(l, -2, kMethodLock);
        lua_pushcfunction(l, &Unlock);
        lua_setfield(l, -2, kMethodUnlock);
        lua_pushcfunction(l, &Erase);
        lua_setfield(l, -2, kMethodErase);
        lua_pushcfunction(l, &ExtractSubset);
        lua_setfield(l, -2, kMethodExtractSubset);
        lua_pushcfunction(l, &IsTransparent);
        lua_setfield(l, -2, kMethodIsTransparent);
    }
    lua_setmetatable(l, -2);

    BonesPixmap *bm = new Bitmap(proxy->ctx());
    lua_pushlightuserdata(l, bm);
    auto ptr_bm = (void **)lua_newuserdata(l, sizeof(BonesPixmap *));
    *ptr_bm = bm;

    lua_settable(l, -3);
    lua_pop(l, 2);
    LuaUtils::PopContext(l);
    lua_pushlightuserdata(l, bm);
    return 1;
}

static int Release(lua_State * l)
{
    if (lua_gettop(l) == 1)
    {
        LuaUtils::PushContext(l);
        lua_getfield(l, -1, kProxy);
        lua_getfield(l, -1, kPath);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
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
    auto path = lua_touserdata(l, -1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
    return static_cast<BonesPixmap *>(path);
}

void PixmapProxy::release(BonesPixmap * pixmap)
{
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodRelease);
    lua_pushlightuserdata(l, pixmap);
    auto count = LuaUtils::SafePCall(l, 1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
}

EngineContext & PixmapProxy::ctx()
{
    return *ctx_;
}

Bitmap::Bitmap(EngineContext & ctx)
{
    ctx_ = &ctx;
}

Bitmap::~Bitmap()
{
    ;
}

bool Bitmap::alloc(const BonesISize & size)
{
    return invokeAlloc(size);
}

bool Bitmap::decode(const void * data, size_t len)
{
    return invokeDecode(data, len);
}

int Bitmap::getWidth() const
{
    return invokeGetWidth();
}

int Bitmap::getHeight() const
{
    return invokeGetHeight();
}

void Bitmap::lock()
{
    invokeLock();
}

void Bitmap::unlock()
{
    invokeUnlock();
}

void Bitmap::erase(BonesColor color)
{
    invokeErase(color);
}

bool Bitmap::extractSubset(BonesPixmap & dst, const BonesIRect & subset)
{
    return invokeExtractSubset(dst, subset);
}

bool Bitmap::isTransparent(int x, int y)
{
    return invokeIsTransparent(x, y);
}


bool Bitmap::invokeAlloc(const BonesISize & size)
{
    if (0 >= size.width || 0 >= size.height)
        return false;

    return bitmap_.allocConfigPixels(SkBitmap::kARGB_8888_Config,
        size.width, size.height, false);
}

bool Bitmap::invokeDecode(const void * data, size_t len)
{
    if (!data || 0 == len)
        return false;
    return SkImageDecoder::DecodeMemory(data, len, &bitmap_, 
        SkBitmap::kARGB_8888_Config, 
        SkImageDecoder::kDecodePixels_Mode);
}

int Bitmap::invokeGetWidth() const
{
    return bitmap_.width();
}

int Bitmap::invokeGetHeight() const
{
    return bitmap_.height();
}

void Bitmap::invokeLock()
{
    bitmap_.lockPixels();
}

void Bitmap::invokeUnlock()
{
    bitmap_.unlockPixels();
}

void Bitmap::invokeErase(BonesColor color)
{
    bitmap_.eraseColor(color);
}

bool Bitmap::invokeExtractSubset(BonesPixmap &dst, const BonesIRect & subset)
{
    SkBitmap bm;
    SkIRect isubset = SkIRect::MakeLTRB(
        subset.left, subset.top,
        subset.right, subset.bottom);
    
    return bitmap_.extractSubset(&(static_cast<Bitmap *>(&dst)->GetBitmap()), isubset);
}

bool Bitmap::invokeIsTransparent(int x, int y)
{
    return ColorGetA(bitmap_.getColor(x, y)) != 0;
}

SkBitmap & Bitmap::GetBitmap()
{
    return bitmap_;
}

}