#include "lua_image.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/image.h"
#include "picture.h"

namespace bones
{

static const char * kMetaTableImage = "__mt_image";

static const char * kMethodSetDirect = "setDirect";
static const char * kMethodSetStretch = "setStretch";
static const char * kMethodSetNine = "setNine";
static const char * kMethodSetContent = "setContent";
static const char * kMethodSetColorMatrix = "setColorMatrix";

//(self, x, y)
static int SetDirect(lua_State * l)
{
    auto count = lua_gettop(l);
    if (count <= 0)
        return 0;
    
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaImage * bob = static_cast<LuaImage *>(
        LuaContext::CallGetCObject(l));
    if (bob)
    {
        BonesPoint * bp = nullptr;
        BonesPoint p;
        if (count >= 3)
        {
            p.x = static_cast<BonesScalar>(lua_tointeger(l, 2));
            p.y = static_cast<BonesScalar>(lua_tointeger(l, 3));
            bp = &p;
        }
        bob->setDirect(bp);
    }
    return 0;
}
//self, l, t, r, b
static int SetStretch(lua_State * l)
{
    auto count = lua_gettop(l);
    if (count <= 0)
        return 0;

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaImage * bob = static_cast<LuaImage *>(
        LuaContext::CallGetCObject(l));
    if (bob)
    {
        BonesRect * br = nullptr;
        BonesRect rect;
        if (count >= 5)
        {
            rect.left = static_cast<BonesScalar>(lua_tointeger(l, 2));
            rect.top = static_cast<BonesScalar>(lua_tointeger(l, 3));
            rect.right = static_cast<BonesScalar>(lua_tointeger(l, 4));
            rect.bottom = static_cast<BonesScalar>(lua_tointeger(l, 5));
            br = &rect;
        }
        bob->setStretch(br);
    }
    return 0;
}

//self, l, t, r, b, cl, ct, cr, cb
static int SetNine(lua_State * l)
{
    auto count = lua_gettop(l);
    if (count <= 0)
        return 0;

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaImage * bob = static_cast<LuaImage *>(
        LuaContext::CallGetCObject(l));
    if (bob)
    {
        BonesRect * br = nullptr;
        BonesRect rect;
        if (count >= 5)
        {
            rect.left = static_cast<BonesScalar>(lua_tointeger(l, 2));
            rect.top = static_cast<BonesScalar>(lua_tointeger(l, 3));
            rect.right = static_cast<BonesScalar>(lua_tointeger(l, 4));
            rect.bottom = static_cast<BonesScalar>(lua_tointeger(l, 5));
            br = &rect;
        }
        BonesRect * cr = nullptr;
        BonesRect center;
        if (count >= 9)
        {
            center.left = static_cast<BonesScalar>(lua_tointeger(l, 6));
            center.top = static_cast<BonesScalar>(lua_tointeger(l, 7));
            center.right = static_cast<BonesScalar>(lua_tointeger(l, 8));
            center.bottom = static_cast<BonesScalar>(lua_tointeger(l, 9));
            cr = &center;
        }
        bob->setNine(br, cr);
    }
    return 0;
}

//self key
static int SetContent(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaImage * bob = static_cast<LuaImage *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        bob->setContent(lua_tostring(l, 2));
    return 0;
}
//self, matrix[20]
//self, a, b, .....
static int SetColorMatrix(lua_State * l)
{
    lua_settop(l, 21);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaImage * bob = static_cast<LuaImage *>(
        LuaContext::CallGetCObject(l));
    if (bob)
    {
        BonesColorMatrix cm;
        if (lua_istable(l, 2))
        {//一个数组
            lua_pushnil(l);
            lua_copy(l, 2, -1);
            for (auto i = 1; i <= 20; ++i)
            {
                lua_pushinteger(l, i);
                lua_gettable(l, -2);
                cm.mat[i - 1] = static_cast<BonesScalar>(lua_tointeger(l, -1));
                lua_pop(l, 1);
            }
        }
        else
        {//20个矩阵数字
            for (auto i = 2; i <= 21; ++i)
                cm.mat[i - 2] = static_cast<BonesScalar>(lua_tointeger(l, i));
        }
        bob->setColorMatrix(cm);
    }
    return 0;
}

LuaImage::LuaImage(Image * ob)
:LuaObject(ob)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaImage::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableImage))
    {
        lua_pushcfunction(l, &SetDirect);
        lua_setfield(l, -2, kMethodSetDirect);
        lua_pushcfunction(l, &SetStretch);
        lua_setfield(l, -2, kMethodSetStretch);
        lua_pushcfunction(l, &SetNine);
        lua_setfield(l, -2, kMethodSetNine);
        lua_pushcfunction(l, &SetContent);
        lua_setfield(l, -2, kMethodSetContent);
        lua_pushcfunction(l, &SetColorMatrix);
        lua_setfield(l, -2, kMethodSetColorMatrix);
    }
}

void LuaImage::setDirect(const BonesPoint * bp)
{
    Point * p = nullptr;
    Point po;
    if (bp)
    {
        po.set(bp->x, bp->y);
        p = &po;
    }
    object_->setDirect(p);
}

void LuaImage::setStretch(const BonesRect * dst)
{
    Rect * d = nullptr;
    Rect dr;
    if (dst)
    {
        dr.setLTRB(dst->left, dst->top, dst->right, dst->bottom);
        d = &dr;
    }
    object_->setStretch(d);
}

void LuaImage::setNine(const BonesRect * dst, const BonesRect * center)
{
    Rect * d = nullptr;
    Rect dr;
    if (dst)
    {
        dr.setLTRB(dst->left, dst->top, dst->right, dst->bottom);
        d = &dr;
    }
    Rect * c = nullptr;
    Rect cr;
    if (center)
    {
        cr.setLTRB(center->left, center->top, center->right, center->bottom);
        c = &cr;
    }
    object_->setNine(d, c);
}

void LuaImage::setContent(const BonesPixmap & pm)
{
    object_->set(static_cast<const Picture *>(&pm)->getPixmap());
}

void LuaImage::setContent(const char * key)
{
    object_->set(key);
}

void LuaImage::setColorMatrix(const BonesColorMatrix & cm)
{
    ColorMatrix m;
    memcpy(&m, &cm, sizeof(m));
    object_->setColorMatrix(m);
}

}