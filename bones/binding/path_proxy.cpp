#include "path_proxy.h"
#include "lua_utils.h"
#include "lua_check.h"
#include "core\logging.h"
#include "SkPath.h"

namespace bones
{

static const char * kPathProxy = "PathProxy";

static const char * kMethodCreate = "create";
static const char * kMethodMoveTo = "moveTo";
static const char * kMethodLineTo = "lineTo";
static const char * kMethodQuadTo = "quadTo";
static const char * kMethodConicTo = "conicTo";
static const char * kMethodCubicTo = "cubicTo";
static const char * kMethodArcTo = "arcTo";
static const char * kMethodClose = "close";
static const char * kMethodRelease = "release";

static SkPath * Cast(void * path)
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
        Scalar x;
        Scalar y;
        LuaUtils::GetPoint(l, x, y);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->moveTo(x, y);
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
        Scalar x;
        Scalar y;
        LuaUtils::GetPoint(l, x, y);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->lineTo(x, y);
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
        Scalar p2x, p2y;
        LuaUtils::PopPoint(l, p2x, p2y);
        Scalar p1x, p1y;
        LuaUtils::PopPoint(l, p1x, p1y);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->quadTo(p1x, p1y, p2x, p2y);
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
        Scalar p2x, p2y;
        LuaUtils::PopPoint(l, p2x, p2y);
        Scalar p1x, p1y;
        LuaUtils::PopPoint(l, p1x, p1y);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->conicTo(p1x, p1y, p2x, p2y, w);
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
        Scalar p3x, p3y;
        LuaUtils::PopPoint(l, p3x, p3y);
        Scalar p2x, p2y;
        LuaUtils::PopPoint(l, p2x, p2y);
        Scalar p1x, p1y;
        LuaUtils::PopPoint(l, p1x, p1y);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->cubicTo(p1x, p1y, p2x, p2y, p3x, p3y);
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
            Scalar sweep = LuaUtils::ToScalar(lua_tonumber(l, 4));
            Scalar start = LuaUtils::ToScalar(lua_tonumber(l, 3));
            lua_pop(l, 2);
            Scalar left, top, right, bottom;
            LuaUtils::PopRect(l, left, top, right, bottom);
            auto path = Cast(lua_touserdata(l, 1));
            if (path)
                path->arcTo(
                SkRect::MakeLTRB(left, top, right, bottom),
                start, sweep, false);
        }
        else
        {
            Scalar radius = LuaUtils::ToScalar(lua_tonumber(l, 4));
            lua_pop(l, 1);
            Scalar p2x, p2y;
            LuaUtils::PopPoint(l, p2x, p2y);
            Scalar p1x, p1y;
            LuaUtils::PopPoint(l, p1x, p1y);
            auto path = Cast(lua_touserdata(l, 1));
            if (path)
                path->arcTo(p1x, p1y, p2x, p2y, radius);
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

    lua_setglobal(state_, kPathProxy);
}

PathProxy::~PathProxy()
{
    lua_pushnil(state_);
    lua_setglobal(state_, kPathProxy);
}

PathProxy::Path PathProxy::create()
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodCreate);
    auto count = LuaUtils::SafePCall(state_, 0);
    auto path = lua_touserdata(state_, -1);
    lua_pop(state_, count + 1);
    return path;
}

void PathProxy::moveTo(Path path, Scalar px, Scalar py)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodMoveTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, px, py);
    auto count = LuaUtils::SafePCall(state_, 2);
    lua_pop(state_, count + 1);
}

void PathProxy::lineTo(Path path, Scalar px, Scalar py)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodLineTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, px, py);
    auto count = LuaUtils::SafePCall(state_, 2);
    lua_pop(state_, count + 1);
}

void PathProxy::quadTo(Path path, Scalar p1x, Scalar p1y,
    Scalar p2x, Scalar p2y)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodQuadTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1x, p1y);
    LuaUtils::PushPoint(state_, p2x, p2y);
    auto count = LuaUtils::SafePCall(state_, 3);
    lua_pop(state_, count + 1);
}

void PathProxy::conicTo(Path path, Scalar p1x, Scalar p1y,
    Scalar p2x, Scalar p2y, Scalar w)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodConicTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1x, p1y);
    LuaUtils::PushPoint(state_, p2x, p2y);
    lua_pushnumber(state_, w);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
}

void PathProxy::cubicTo(Path path, Scalar p1x, Scalar p1y,
    Scalar p2x, Scalar p2y, Scalar p3x, Scalar p3y)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodCubicTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1x, p1y);
    LuaUtils::PushPoint(state_, p2x, p2y);
    LuaUtils::PushPoint(state_, p3x, p3y);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
}

void PathProxy::arcTo(Path path, Scalar l, Scalar t, Scalar r, Scalar b,
    Scalar startAngle, Scalar sweepAngle)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getglobal(state_, kPathProxy);
    lua_getfield(state_, -1, kMethodArcTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushRect(state_, l, t, r, b);
    lua_pushnumber(state_, startAngle);
    lua_pushnumber(state_, sweepAngle);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
}

void PathProxy::arcTo(Path path, Scalar p1x, Scalar p1y,
    Scalar p2x, Scalar p2y, Scalar radius)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getfield(state_, -1, kMethodArcTo);
    lua_pushlightuserdata(state_, path);
    LuaUtils::PushPoint(state_, p1x, p1y);
    LuaUtils::PushPoint(state_, p2x, p2y);
    lua_pushnumber(state_, radius);
    auto count = LuaUtils::SafePCall(state_, 4);
    lua_pop(state_, count + 1);
}

void PathProxy::close(Path path)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getfield(state_, -1, kMethodClose);
    lua_pushlightuserdata(state_, path);
    auto count = LuaUtils::SafePCall(state_, 1);
    lua_pop(state_, count + 1);
}

void PathProxy::release(Path path)
{
    LUA_STACK_AUTO_CHECK(state_);
    lua_getfield(state_, -1, kMethodRelease);
    lua_pushlightuserdata(state_, path);
    auto count = LuaUtils::SafePCall(state_, 1);
    lua_pop(state_, count + 1);

}

}