#include "lua_shape.h"
#include "lua_context.h"
#include "lua_check.h"
#include "utils.h"
#include "core/logging.h"

namespace bones
{
static const char * kMetaTableShape = "__mt_shape";

static const char * kMethodSetStroke = "setStroke";
static const char * kMethodsetStrokeEffect = "setStrokeEffect";
static const char * kMethodsetStrokeWidth = "setStrokeWidth";
static const char * kMethodsetColor = "setColor";
static const char * kMethodsetCircle = "setCircle";
static const char * kMethodsetRect = "setRect";
static const char * kMethodsetLine = "setLine";
static const char * kMethodsetPoint = "setPoint";
static const char * kMethodsetOval = "setOval";
static const char * kMethodsetArc = "setArc";

static int SetStroke(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
        shape->setStroke(!!(lua_toboolean(l, 2)));
    return 0;
}

static int SetStrokeEffect(lua_State * l)
{
    auto count = lua_gettop(l);
    if (count > 2)
    {
        auto intervals_count = static_cast<size_t>(lua_tointeger(l, 2));
        lua_settop(l, intervals_count + 3);
        lua_pushnil(l);
        lua_copy(l, 1, -1);
        LuaShape * shape = static_cast<LuaShape *>(
            LuaContext::CallGetCObject(l));
        if (shape)
        {
            std::vector<BonesScalar> intervals;
            for (size_t i = 1; i <= intervals_count; ++i)
                intervals.push_back(Utils::ToBonesScalar(lua_tonumber(l, 2 + i)));
            shape->setStrokeEffect(intervals_count, &intervals[0],
                Utils::ToBonesScalar(lua_tonumber(l, 3 + intervals_count)));
        }

    }

    return 0;
}

static int SetStrokeWidth(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
        shape->setStrokeWidth(Utils::ToBonesScalar(lua_tonumber(l, 2)));
    return 0;
}

static int SetColor(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        if (lua_isinteger(l, 2))
            shape->setColor(static_cast<BonesColor>(lua_tointeger(l, 2)));
        else if (lua_islightuserdata(l, 2))
            shape->setColor(static_cast<BonesShader>(lua_touserdata(l, 2)));
    }
    return 0;
}

static int SetCircle(lua_State * l)
{
    lua_settop(l, 4);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        BonesPoint center = { 
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)) };
        shape->setCircle(center, Utils::ToBonesScalar(lua_tonumber(l, 4)));
    }
    return 0;
}

static int SetRect(lua_State * l)
{//(self, rx, ry, l, t, r, b) || (self)
    auto count = lua_gettop(l);
    lua_settop(l, 7);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        if (count == 1)
            shape->setRect(0, 0, nullptr);
        else if (count == 3)
            shape->setRect(
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)), nullptr);
        else
        {
            BonesRect rect = {
                Utils::ToBonesScalar(lua_tonumber(l, 4)),
                Utils::ToBonesScalar(lua_tonumber(l, 5)),
                Utils::ToBonesScalar(lua_tonumber(l, 6)),
                Utils::ToBonesScalar(lua_tonumber(l, 7)) };
            shape->setRect(
                Utils::ToBonesScalar(lua_tonumber(l, 2)),
                Utils::ToBonesScalar(lua_tonumber(l, 3)), &rect);
        }
    }

    return 0;
}

static int SetLine(lua_State * l)
{
    lua_settop(l, 5);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        BonesPoint start = {
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)) };
        BonesPoint end = {
            Utils::ToBonesScalar(lua_tonumber(l, 4)),
            Utils::ToBonesScalar(lua_tonumber(l, 5)) };
        shape->setLine(start, end);
    }
    return 0;
}

static int SetPoint(lua_State * l)
{
    lua_settop(l, 3);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        BonesPoint pt = {
            Utils::ToBonesScalar(lua_tonumber(l, 2)),
            Utils::ToBonesScalar(lua_tonumber(l, 3)) };
        shape->setPoint(pt);
    }
    return 0;
}

static int SetOval(lua_State * l)
{
    auto count = lua_gettop(l);
    lua_settop(l, 5);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        if (count == 1)
            shape->setOval(nullptr);
        else
        {
            BonesRect oval = { 
                Utils::ToBonesScalar(lua_tonumber(l, 2)),
                Utils::ToBonesScalar(lua_tonumber(l, 3)),
                Utils::ToBonesScalar(lua_tonumber(l, 4)),
                Utils::ToBonesScalar(lua_tonumber(l, 5)) };
            shape->setOval(&oval);
        }
    }
    return 0;
}

static int SetArc(lua_State * l)
{
    auto count = lua_gettop(l);
    lua_settop(l, 8);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaShape * shape = static_cast<LuaShape *>(
        LuaContext::CallGetCObject(l));
    if (shape)
    {
        auto start = Utils::ToBonesScalar(lua_tonumber(l, 2));
        auto sweep = Utils::ToBonesScalar(lua_tonumber(l, 3));
        auto center = !!lua_toboolean(l, 4);
        if (count == 4)
            shape->setArc(start, sweep, center, nullptr);
        else
        {
            BonesRect oval = {
                Utils::ToBonesScalar(lua_tonumber(l, 5)),
                Utils::ToBonesScalar(lua_tonumber(l, 6)),
                Utils::ToBonesScalar(lua_tonumber(l, 7)),
                Utils::ToBonesScalar(lua_tonumber(l, 8)) };
            shape->setArc(start, sweep, center, &oval);
        }
    }
    return 0;
}

LuaShape::LuaShape(Shape * co)
:LuaObject(co)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaShape::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableShape))
    {
        lua_pushcfunction(l, &SetStroke);
        lua_setfield(l, -2, kMethodSetStroke);

        lua_pushcfunction(l, &SetStrokeEffect);
        lua_setfield(l, -2, kMethodsetStrokeEffect);
        lua_pushcfunction(l, &SetStrokeWidth);
        lua_setfield(l, -2, kMethodsetStrokeWidth);

        lua_pushcfunction(l, &SetColor);
        lua_setfield(l, -2, kMethodsetColor);

        lua_pushcfunction(l, &SetCircle);
        lua_setfield(l, -2, kMethodsetCircle);

        lua_pushcfunction(l, &SetRect);
        lua_setfield(l, -2, kMethodsetRect);

        lua_pushcfunction(l, &SetLine);
        lua_setfield(l, -2, kMethodsetLine);

        lua_pushcfunction(l, &SetPoint);
        lua_setfield(l, -2, kMethodsetPoint);

        lua_pushcfunction(l, &SetOval);
        lua_setfield(l, -2, kMethodsetOval);

        lua_pushcfunction(l, &SetArc);
        lua_setfield(l, -2, kMethodsetArc);
    }
}

void LuaShape::setStroke(bool stroke)
{
    object_->setStyle(stroke ? Shape::kStroke : Shape::kFill);
}

void LuaShape::setStrokeEffect(size_t count,
    BonesScalar * intervals,
    BonesScalar offset)
{
    auto effect = Core::createDashEffect(count, intervals, offset);
    object_->setStrokeEffect(effect);
    Core::destroyEffect(effect);
}

void LuaShape::setStrokeWidth(BonesScalar stroke_width)
{
    object_->setStrokeWidth(stroke_width);
}

void LuaShape::setColor(BonesColor color)
{
    object_->setColor(color);
}

void LuaShape::setColor(BonesShader shader)
{
    object_->setColor(static_cast<SkShader *>(shader));
}


void LuaShape::setCircle(const BonesPoint & center, BonesScalar radius)
{
    Point p;
    p.set(center.x, center.y);
    object_->set(p, radius);
}

void LuaShape::setRect(BonesScalar rx, BonesScalar ry,
    const BonesRect * rect)
{
    Rect * r = nullptr;
    Rect re;
    if (rect)
    {
        re.setLTRB(rect->left, rect->top, rect->right, rect->bottom);
        r = &re;
    }
    object_->set(rx, ry, r);
}

void LuaShape::setLine(const BonesPoint & start, const BonesPoint & end)
{
    Point pts, pte;
    pts.set(start.x, start.y);
    pte.set(end.x, end.y);
    object_->set(pts, pte);
}

void LuaShape::setPoint(const BonesPoint & pt)
{
    Point p;
    p.set(pt.x, pt.y);
    object_->set(p);
}

void LuaShape::setOval(const BonesRect * oval)
{
    Rect * r = nullptr;
    Rect re;
    if (oval)
    {
        re.setLTRB(oval->left, oval->top, oval->right, oval->bottom);
        r = &re;
    }
    object_->set(r);
}

void LuaShape::setArc(
    BonesScalar start,
    BonesScalar sweep,
    bool use_center,
    const BonesRect * oval)
{
    Rect * r = nullptr;
    Rect re;
    if (oval)
    {
        re.setLTRB(oval->left, oval->top, oval->right, oval->bottom);
        r = &re;
    }
    object_->set(r, start, sweep, use_center);
}

void LuaShape::setPath(BonesPath path)
{
    object_->set(*Utils::ToSkPath(path));
}

}
