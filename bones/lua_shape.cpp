#include "lua_shape.h"
#include "lua_context.h"
#include "lua_check.h"

#include "core/logging.h"

namespace bones
{
static const char * kMetaTableShape = "__mt_shape";

static const char * kMethodSetStroke = "setStroke";
static const char * kMethodsetStrokeEffect = "setStrokeEffect";
static const char * kMethodsetStrokeWidth = "setStrokeWidth";
static const char * kMethodsetColor = "setColor";
static const char * kMethodsetBorder = "setBorder";
static const char * kMethodsetCircle = "setCircle";
static const char * kMethodsetRect = "setRect";
static const char * kMethodsetLine = "setLine";
static const char * kMethodsetPoint = "setPoint";

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
            static_cast<BonesScalar>(lua_tonumber(l, 2)),
            static_cast<BonesScalar>(lua_tonumber(l, 3)), nullptr);
        else
        {
            BonesRect rect = {
                static_cast<BonesScalar>(lua_tonumber(l, 4)),
                static_cast<BonesScalar>(lua_tonumber(l, 5)),
                static_cast<BonesScalar>(lua_tonumber(l, 6)),
                static_cast<BonesScalar>(lua_tonumber(l, 7)) };
            shape->setRect(
                static_cast<BonesScalar>(lua_tonumber(l, 2)),
                static_cast<BonesScalar>(lua_tonumber(l, 3)), &rect);
        }
    }

    return 0;
}

LuaShape::LuaShape(Shape * co)
:LuaObject(co), notify_(nullptr)
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

        lua_pushcfunction(l, &SetRect);
        lua_setfield(l, -2, kMethodsetRect);

        lua_pushcfunction(l, &SetColor);
        lua_setfield(l, -2, kMethodsetColor);
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
    object_->setShader(static_cast<SkShader *>(shader));
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

}
