#include "path_proxy.h"
#include "lua_utils.h"
#include "lua_check.h"
#include "helper\logging.h"
#include "SkPath.h"
#include <assert.h>

namespace bones
{

static const char * kProxy = "PathProxy";
static const char * kPath = "_path_";
static const char * kMetaTable = "_mt_path_";

static const char * kMethodCreate = "create";
static const char * kMethodMoveTo = "moveTo";
static const char * kMethodLineTo = "lineTo";
static const char * kMethodQuadTo = "quadTo";
static const char * kMethodConicTo = "conicTo";
static const char * kMethodCubicTo = "cubicTo";
static const char * kMethodArcTo = "arcTo";
static const char * kMethodClose = "close";
static const char * kMethodRelease = "release";

class Path : public BonesPath
{
public:
    Path(EngineContext * ctx);

    virtual ~Path();

    void moveTo(const BonesPoint & p) override;

    void lineTo(const BonesPoint & p) override;

    void quadTo(const BonesPoint & p1,
        const BonesPoint & p2) override;

    void conicTo(const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar w) override;

    void cubicTo(const BonesPoint & p1,
        const BonesPoint & p2, const BonesPoint & p3) override;

    void arcTo(const BonesRect & oval,
        BonesScalar startAngle, BonesScalar sweepAngle) override;

    void arcTo(const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar radius) override;

    void close() override;
public:
    //methods only script call;
    void scriptMoveTo(const BonesPoint & p);

    void scriptLineTo(const BonesPoint & p);

    void scriptQuadTo(const BonesPoint & p1,
        const BonesPoint & p2);

    void scriptConicTo(const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar w);

    void scriptCubicTo(const BonesPoint & p1,
        const BonesPoint & p2, const BonesPoint & p3);

    void scriptArcTo(const BonesRect & oval,
        BonesScalar startAngle, BonesScalar sweepAngle);

    void scriptArcTo(const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar radius);

    void scriptClose();
private:
    SkPath * path_;
    EngineContext * ctx_;
};

static inline Path * Cast(void * path)
{
    return static_cast<Path *>(*(void **)path);
}


//path, x, y
static int MoveTo(lua_State * l)
{
    auto count = lua_gettop(l);
    if (3 == count)
    {
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
        {
            BonesPoint pt = { 
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)) };
            path->scriptMoveTo(pt);
        }

    }
    else if ( 2 == count)
    {// so utils
        BonesPoint pt;
        LuaUtils::GetPoint(l, -1, pt);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->scriptMoveTo(pt);
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
        {
            BonesPoint pt = {
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)) };
            path->scriptLineTo(pt);
        }

    }
    else if (2 == count)
    {
        BonesPoint pt;
        LuaUtils::GetPoint(l, -1, pt);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->scriptLineTo(pt);
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
            BonesPoint p1 = {
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)) };
            BonesPoint p2 = { 
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)) };
            path->scriptQuadTo(p1, p2);
        }
    }
    else if (3 == count)
    {
        BonesPoint p2;
        BonesPoint p1;
        LuaUtils::PopPoint(l, p2);
        LuaUtils::PopPoint(l, p1);
        auto path = Cast(lua_touserdata(l, 1));
        if (path)
            path->scriptQuadTo(p1, p2);
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
        {
            BonesPoint p1 = {
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)) };
            BonesPoint p2 = {
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)) };
            path->scriptConicTo(p1, p2,
                LuaUtils::ToScalar(lua_tonumber(l, 6)));
        }

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
            path->scriptConicTo(p1, p2, w);
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
        {
            BonesPoint p1 = {
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)) };
            BonesPoint p2 = {
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)) };
            BonesPoint p3 = {
                LuaUtils::ToScalar(lua_tonumber(l, 6)),
                LuaUtils::ToScalar(lua_tonumber(l, 7)) };
            path->scriptCubicTo(p1, p2, p3);
        }            
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
            path->scriptCubicTo(p1, p2, p3);
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
        {
            BonesRect r = { 
                LuaUtils::ToScalar(lua_tonumber(l, 2)),
                LuaUtils::ToScalar(lua_tonumber(l, 3)),
                LuaUtils::ToScalar(lua_tonumber(l, 4)),
                LuaUtils::ToScalar(lua_tonumber(l, 5)) };
            path->scriptArcTo(r,
                LuaUtils::ToScalar(lua_tonumber(l, 6)),
                LuaUtils::ToScalar(lua_tonumber(l, 7)));
        }
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
                path->scriptArcTo(r, start, sweep);
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
                path->scriptArcTo(p1, p2, radius);
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


static int GC(lua_State * l)
{
    int count = lua_gettop(l);
    if (count == 1)
    {
        auto ptr_path = (void **)lua_touserdata(l, 1);
        if (ptr_path)
        {
            auto path = static_cast<Path *>(*ptr_path);
            if (path)
                delete path;
        }
    }
    return 0;
}

static int Create(lua_State * l)
{
    LuaUtils::PushContext(l);

    lua_getfield(l, -1, kProxy);
    auto proxy_ = static_cast<PathProxy *>(LuaUtils::GetFieldCObject(l));
    lua_pop(l, 1);
    lua_getfield(l, -1, kPath);

    auto path = new Path(proxy_->ctx());
    lua_pushlightuserdata(l, path);
    auto ptr_path = (void **)lua_newuserdata(l, sizeof(Path *));
    *ptr_path = path;
    //set metatable
    luaL_getmetatable(l, kMetaTable);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        LuaUtils::CreateMetaTable(l, kMetaTable, &GC);

        lua_pushcfunction(l, &MoveTo);
        lua_setfield(l, -2, kMethodMoveTo);
        lua_pushcfunction(l, &LineTo);
        lua_setfield(l, -2, kMethodLineTo);
        lua_pushcfunction(l, &QuadTo);
        lua_setfield(l, -2, kMethodQuadTo);
        lua_pushcfunction(l, &ConicTo);
        lua_setfield(l, -2, kMethodConicTo);
        lua_pushcfunction(l, &CubicTo);
        lua_setfield(l, -2, kMethodCubicTo);
        lua_pushcfunction(l, &ArcTo);
        lua_setfield(l, -2, kMethodArcTo);
        lua_pushcfunction(l, &Close);
        lua_setfield(l, -2, kMethodClose);

    }
    lua_setmetatable(l, -2);

    lua_settable(l, -3);

    lua_pop(l, 2);

    LuaUtils::PopContext(l);

    lua_pushlightuserdata(l, path);
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

PathProxy::PathProxy(EngineContext * ctx)
{
    ctx_ = ctx;
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_newtable(l);
    LuaUtils::SetFieldCObject(l, this);

    lua_pushcfunction(l, &Create);
    lua_setfield(l, -2, kMethodCreate);
    lua_pushcfunction(l, &Release);
    lua_setfield(l, -2, kMethodRelease);
    //store path
    lua_newtable(l);
    lua_setfield(l, -2, kPath);

    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

PathProxy::~PathProxy()
{
    auto l = ctx_->State();
    LuaUtils::PushContext(l);
    //remove path table
    lua_getfield(l, -1, kProxy);
    lua_pushnil(l);
    lua_setfield(l, -2, kPath);
    lua_pop(l, 1);
    //remvoe proxy table
    lua_pushnil(l);
    lua_setfield(l, -2, kProxy);
    LuaUtils::PopContext(l);
}

BonesPath * PathProxy::create()
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
    return static_cast<BonesPath *>(path);
}

void PathProxy::release(BonesPath * path)
{
    auto l = ctx_->State();
    LUA_STACK_AUTO_CHECK(l);
    LuaUtils::PushContext(l);
    lua_getfield(l, -1, kProxy);
    lua_getfield(l, -1, kMethodRelease);
    lua_pushlightuserdata(l, path);
    auto count = LuaUtils::SafePCall(l, 1);
    lua_pop(l, count + 1);
    LuaUtils::PopContext(l);
}

Path::Path(EngineContext * ctx)
{
    ctx_ = ctx;
    path_ = new SkPath;
}

Path::~Path()
{
    delete path_;
}

void Path::moveTo(const BonesPoint & p)
{
    scriptMoveTo(p);
}

void Path::lineTo(const BonesPoint & p)
{
    scriptLineTo(p);
}

void Path::quadTo(const BonesPoint & p1,
    const BonesPoint & p2)
{
    scriptQuadTo(p1, p2);
}

void Path::conicTo(const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar w)
{
    scriptConicTo(p1, p2, w);
}

void Path::cubicTo(const BonesPoint & p1,
    const BonesPoint & p2, const BonesPoint & p3)
{
    scriptCubicTo(p1, p2, p3);
}

void Path::arcTo(const BonesRect & oval,
    BonesScalar startAngle, BonesScalar sweepAngle)
{
    scriptArcTo(oval, startAngle, sweepAngle);
}

void Path::arcTo(const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar radius)
{
    scriptArcTo(p1, p2, radius);
}

void Path::close()
{
    scriptClose();
}

void Path::scriptMoveTo(const BonesPoint & p)
{
    path_->moveTo(p.x, p.y);
}

void Path::scriptLineTo(const BonesPoint & p)
{
    path_->lineTo(p.x, p.y);
}

void Path::scriptQuadTo(const BonesPoint & p1,
    const BonesPoint & p2)
{
    path_->quadTo(p1.x, p1.y, p2.x, p2.y);
}

void Path::scriptConicTo(const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar w)
{
    path_->conicTo(p1.x, p1.y, p2.x, p2.y, w);
}

void Path::scriptCubicTo(const BonesPoint & p1,
    const BonesPoint & p2, const BonesPoint & p3)
{
    path_->cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
}

void Path::scriptArcTo(const BonesRect & oval,
    BonesScalar startAngle, BonesScalar sweepAngle)
{
    path_->arcTo(SkRect::MakeLTRB(oval.left, oval.top, oval.top, oval.bottom),
        startAngle, sweepAngle, false);
}

void Path::scriptArcTo(const BonesPoint & p1,
    const BonesPoint & p2, BonesScalar radius)
{
    path_->arcTo(p1.x, p1.y, p2.x, p2.y, radius);
}

void Path::scriptClose()
{
    path_->close();
}

}