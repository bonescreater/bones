#include "path_proxy.h"
#include "lua_utils.h"
#include "lua_check.h"
#include "helper\logging.h"
#include "SkPath.h"

namespace bones
{

static const char * kProxy = "PathProxy";

static const char * kMethodCreate = "create";
static const char * kMethodMoveTo = "moveTo";
static const char * kMethodLineTo = "lineTo";
static const char * kMethodQuadTo = "quadTo";
static const char * kMethodConicTo = "conicTo";
static const char * kMethodCubicTo = "cubicTo";
static const char * kMethodArcTo = "arcTo";
static const char * kMethodClose = "close";
static const char * kMethodRelease = "release";

static inline SkPath * Cast(void * path)
{
    return static_cast<SkPath *>(path);
}

static int Create(lua_State * l)
{
    lua_pushlightuserdata(l, new SkPath);
    return 1;
}
//path, x, y
static int MoveTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (3 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->moveTo(
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)));
    }
    else if ( 2 == count)
    {// so utils
        BonesPoint pt;
        LuaUtils::GetPoint(l, -1, pt);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->moveTo(pt.x, pt.y);
    }
    else
        BLG_ERROR << "path." << kMethodMoveTo << " error";
    return 0;
}
//path, x, y
static int LineTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (3 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->lineTo(
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)));
    }
    else if (2 == count)
    {
        BonesPoint pt;
        LuaUtils::GetPoint(l, -1, pt);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->lineTo(pt.x, pt.y);
    }
    else
        BLG_ERROR << "path." << kMethodLineTo << " error";
    return 0;
}

static int QuadTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (5 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
        {
            path->quadTo(
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)),
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)));
        }
    }
    else if (3 == count)
    {
        BonesPoint p2;
        LuaUtils::PopPoint(l, p2);
        BonesPoint p1;
        LuaUtils::PopPoint(l, p1);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->quadTo(p1.x, p1.y, p2.x, p2.y);
    }
    else
        BLG_ERROR << "path." << kMethodQuadTo << " error";
    return 0;
}

static int ConicTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (6 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->conicTo(
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)),
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)),
                LuaUtils::ToScalar(lua_tonumber(l, 6)));
    }
    else if (4 == count)
    {
        auto w = LuaUtils::ToScalar(lua_tonumber(l, 4));
        lua_pop(l, 1);
        BonesPoint p2;
        LuaUtils::PopPoint(l, p2);
        BonesPoint p1;
        LuaUtils::PopPoint(l, p1);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->conicTo(p1.x, p1.y, p2.x, p2.y, w);
    }
    else
        BLG_ERROR << "path." << kMethodConicTo << " error";
    return 0;
}

static int CubicTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (7 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->cubicTo(
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)),
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)),
                LuaUtils::ToScalar(lua_tonumber(l, 6)),
                LuaUtils::ToScalar(lua_tonumber(l, 7)));
    }
    else if (4 == count)
    {
        BonesPoint p3;
        LuaUtils::PopPoint(l, p3);
        BonesPoint p2;
        LuaUtils::PopPoint(l, p2);
        BonesPoint p1;
        LuaUtils::PopPoint(l, p1);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
    }
    else
        BLG_ERROR << "path." << kMethodCubicTo << " error";
    return 0;
}

static int ArcTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (7 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->arcTo(
                SkRect::MakeLTRB(
                    LuaUtils::ToScalar(lua_tonumber(l, 2)), 
                    LuaUtils::ToScalar(lua_tonumber(l, 3)), 
                    LuaUtils::ToScalar(lua_tonumber(l, 4)), 
                    LuaUtils::ToScalar(lua_tonumber(l, 5))),
                    LuaUtils::ToScalar(lua_tonumber(l, 6)), 
                    LuaUtils::ToScalar(lua_tonumber(l, 7)), 
                    false);
    }
    else if (4 == count)
    {
        if (!lua_istable(l, 3))
        {
            auto sweep = LuaUtils::ToScalar(lua_tonumber(l, 4));
            auto start = LuaUtils::ToScalar(lua_tonumber(l, 3));
            lua_pop(l, 2);
            BonesRect r; 
            LuaUtils::PopRect(l, r);
            auto path = Cast(lua_touserdata(l, 1));
            if (path)
                path->arcTo(
                SkRect::MakeLTRB(r.left, r.top, r.right, r.bottom),
                start, sweep, false);
        }
        else
        {
            auto radius = LuaUtils::ToScalar(lua_tonumber(l, 4));
            lua_pop(l, 1);
            BonesPoint p2;
            LuaUtils::PopPoint(l, p2);
            BonesPoint p1;
            LuaUtils::PopPoint(l, p1);
            auto path = Cast(lua_touserdata(l, 1));
            if (path)
                path->arcTo(p1.x, p1.y, p2.x, p2.y, radius);
        }

    }
    else
        BLG_ERROR << "path." << kMethodArcTo << " error";
    return 0;
}

static int Close(lua_State * l)
{
    if (lua_gettop(l) == 1)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->close();
    }
    return 0;
}

static int Release(lua_State * l)
{
    if (lua_gettop(l) == 1)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            delete path;
    }
    return 0;
}

PathProxy::PathProxy(lua_State * s)
{
    state_ = s;
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_newtable(state_);

    lua_pushcfunction(state_, &Create);
    lua_setfield(state_, -2, kMethodCreate);
    lua_pushcfunction(state_, &MoveTo);
    lua_setfield(state_, -2, kMethodMoveTo);
    lua_pushcfunction(state_, &LineTo);
    lua_setfield(state_, -2, kMethodLineTo);
    lua_pushcfunction(state_, &QuadTo);
    lua_setfield(state_, -2, kMethodQuadTo);
    lua_pushcfunction(state_, &ConicTo);
    lua_setfield(state_, -2, kMethodConicTo);
    lua_pushcfunction(state_, &CubicTo);
    lua_setfield(state_, -2, kMethodCubicTo);
    lua_pushcfunction(state_, &ArcTo);
    lua_setfield(state_, -2, kMethodArcTo);
    lua_pushcfunction(state_, &Close);
    lua_setfield(state_, -2, kMethodClose);
    lua_pushcfunction(state_, &Release);
    lua_setfield(state_, -2, kMethodRelease);

    lua_setfield(state_, -2, kProxy);
    LuaUtils::PopContext(state_);
}

PathProxy::~PathProxy()
{
    LuaUtils::PushContext(state_);
    lua_pushnil(state_);
    lua_setfield(state_, -2, kProxy);
    LuaUtils::PopContext(state_);
}

BonesPath PathProxy::create()
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodCreate);
    auto count = LuaUtils::SafePCall(state_, 0);
    auto path = lua_touserdata(state_, -1);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
    return path;
}

void PathProxy::moveTo(BonesPath path, const BonesPoint & p)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodMoveTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p);
    auto count = LuaUtils::SafePCall(state_, 2);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::lineTo(BonesPath path, const BonesPoint & p)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodLineTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p);
    auto count = LuaUtils::SafePCall(state_, 2);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::quadTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodQuadTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1);
    LuaUtils::PushPoint(state_, p2);
    auto count = LuaUtils::SafePCall(state_, 3);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::conicTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar w)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodConicTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1);
    LuaUtils::PushPoint(state_, p2);
    lua_pushnumber(state_, w);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::cubicTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, const BonesPoint & p3)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodCubicTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1);
    LuaUtils::PushPoint(state_, p2);
    LuaUtils::PushPoint(state_, p3);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::arcTo(BonesPath path, const BonesRect & oval,
    BonesScalar startAngle, BonesScalar sweepAngle)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodArcTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushRect(state_, oval);
    lua_pushnumber(state_, startAngle);
    lua_pushnumber(state_, sweepAngle);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::arcTo(BonesPath path, const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar radius)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodArcTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1);
    LuaUtils::PushPoint(state_, p2);
    lua_pushnumber(state_, radius);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::close(BonesPath path)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodClose);
    lua_pushlightuserdata(state_, path);
    auto count = LuaUtils::SafePCall(state_, 1);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

void PathProxy::release(BonesPath path)
{
    LUA_STACK_AUTO_CHECK(state_);
    LuaUtils::PushContext(state_);
    lua_getfield(state_, -1, kProxy);
    lua_getfield(state_, -1, kMethodRelease);
    lua_pushlightuserdata(state_, path);
    auto count = LuaUtils::SafePCall(state_, 1);
    lua_pop(state_, count + 1);
    LuaUtils::PopContext(state_);
}

}