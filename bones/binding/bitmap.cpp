#include "bitmap.h"
#include "core/color.h"
#include "SkBitmap.h"
#include "SkImageDecoder.h"
#include "lua_utils.h"

namespace bones
{
static const char * kMetaTable = "_mt_pixmap_";


static const char * kMethodAlloc = "alloc";
static const char * kMethodDecode = "decode";
static const char * kMethodGetWidth = "getWidth";
static const char * kMethodGetHeight = "getHeight";
static const char * kMethodLock = "lock";
static const char * kMethodUnlock = "unlock";
static const char * kMethodErase = "erase";
static const char * kMethodExtractSubset = "extractSubset";
static const char * kMethodIsTransparent = "isTransparent";
static const char * kMethodIsValid = "isValid";

static int Alloc(lua_State * l)
{
    auto count = lua_gettop(l);
    bool ret = false;
    if (3 == count)
    {//self, w, h
        BonesISize size = {
            LuaUtils::IntFromInteger(l, 2),
            LuaUtils::IntFromInteger(l, 3) };
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            ret = bm->invokeAlloc(size);
    }
    else if (2 == count)
    {//self, table
        BonesISize size;
        LuaUtils::GetISize(l, 2, size);
        auto bm = Bitmap::Cast(l, 1);
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
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            ret = bm->invokeDecode(
            lua_tostring(l, 2),
            static_cast<size_t>(LuaUtils::IntFromInteger(l, 3)));
    }
    else if (2 == count)
    {//self, lstr
        size_t len = 0;
        auto data = lua_tolstring(l, 2, &len);
        auto bm = Bitmap::Cast(l, 1);
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
        auto bm = Bitmap::Cast(l, 1);
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
        auto bm = Bitmap::Cast(l, 1);
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
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            bm->lock();
    }
    return 0;
}

static int Unlock(lua_State * l)
{
    if (1 == lua_gettop(l))
    {
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            bm->invokeUnlock();
    }
    return 0;
}

static int Erase(lua_State * l)
{
    if (2 == lua_gettop(l))
    {
        auto bm = Bitmap::Cast(l, 1);
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
        auto bm = Bitmap::Cast(l, 1);
        auto dst = Bitmap::Cast(l, 2);
        BonesIRect iret;
        LuaUtils::GetIRect(l, 3, iret);
        if (bm && dst)
            ret = bm->invokeExtractSubset(*dst, iret);
    }
    else if (6 == lua_gettop(l))
    {//self, dst, left, top, right, bottom
        BonesIRect iret;
        iret.bottom = LuaUtils::IntFromInteger(l, 6);
        iret.right = LuaUtils::IntFromInteger(l, 5);
        iret.top = LuaUtils::IntFromInteger(l, 4);
        iret.left = LuaUtils::IntFromInteger(l, 3);
        auto bm = Bitmap::Cast(l, 1);
        auto dst = Bitmap::Cast(l, 2);
        if (bm && dst)
            ret = bm->invokeExtractSubset(*dst, iret);
    }

    lua_pushboolean(l, ret);
    return 1;
}

static int IsTransparent(lua_State * l)
{
    bool bret = false;
    if (3 == lua_gettop(l))
    {
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            bret = bm->invokeIsTransparent(
            LuaUtils::IntFromInteger(l, 2),
            LuaUtils::IntFromInteger(l, 3));
    }
    lua_pushboolean(l, bret);
    return 1;
}

static int IsValid(lua_State * l)
{
    bool bret = false;

    if (1 == lua_gettop(l))
    {
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            bret = bm->invokeIsValid();
    }
    lua_pushboolean(l, bret);
    return 1;
}
static int GC(lua_State *l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        auto bm = Bitmap::Cast(l, 1);
        if (bm)
            delete bm;
    }
    return 0;
}

Bitmap::Bitmap(EngineContext & ctx)
{
    ctx_ = &ctx;
    bitmap_ = new SkBitmap;
}

Bitmap::~Bitmap()
{
    delete bitmap_;
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

bool Bitmap::isValid()  const
{
    return invokeIsValid();
}

static void GetLuaMetaTable(lua_State * l)
{
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
        lua_pushcfunction(l, IsValid);
        lua_setfield(l, -2, kMethodIsValid);
    }
}

Bitmap * Bitmap::CreateLuaNewUserData(lua_State * l, EngineContext & ctx)
{
    Bitmap * bm = new Bitmap(ctx);
    auto ptr_bm = (void **)lua_newuserdata(l, sizeof(BonesPixmap *));
    *ptr_bm = static_cast<BonesPixmap *>(bm);
    GetLuaMetaTable(l);
    lua_setmetatable(l, -2);
    return bm;
}

Bitmap * Bitmap::Cast(lua_State *l, int idx)
{
    auto path = lua_touserdata(l, idx);
    if (!path)
        return nullptr;
    return static_cast<Bitmap *>(static_cast<BonesPixmap *>(*(void **)path));
}

bool Bitmap::invokeAlloc(const BonesISize & size)
{
    if (0 >= size.width || 0 >= size.height)
        return false;

    return bitmap_->allocConfigPixels(SkBitmap::kARGB_8888_Config,
        size.width, size.height, false);
}

bool Bitmap::invokeDecode(const void * data, size_t len)
{
    if (!data || 0 == len)
        return false;
    return SkImageDecoder::DecodeMemory(data, len, bitmap_,
        SkBitmap::kARGB_8888_Config,
        SkImageDecoder::kDecodePixels_Mode);
}

int Bitmap::invokeGetWidth() const
{
    return bitmap_->width();
}

int Bitmap::invokeGetHeight() const
{
    return bitmap_->height();
}

void Bitmap::invokeLock()
{
    bitmap_->lockPixels();
}

void Bitmap::invokeUnlock()
{
    bitmap_->unlockPixels();
}

void Bitmap::invokeErase(BonesColor color)
{
    bitmap_->eraseColor(color);
}

bool Bitmap::invokeExtractSubset(BonesPixmap &dst, const BonesIRect & subset)
{
    SkBitmap bm;
    SkIRect isubset = SkIRect::MakeLTRB(
        subset.left, subset.top,
        subset.right, subset.bottom);

    return bitmap_->extractSubset(&(static_cast<Bitmap *>(&dst)->GetBitmap()), isubset);
}

bool Bitmap::invokeIsTransparent(int x, int y)
{
    return ColorGetA(bitmap_->getColor(x, y)) != 0;
}

bool Bitmap::invokeIsValid()  const
{
    return bitmap_->drawsNothing();
}

SkBitmap & Bitmap::GetBitmap()
{
    return *bitmap_;
}

Bitmap & Bitmap::operator = (const Bitmap & right)
{
    *this->bitmap_ = *right.bitmap_;
    return *this;
}

}