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

LuaShape::LuaShape(Shape * co)
:LuaObject(co), notify_(nullptr)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaShape::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableShape);
}

void LuaShape::setStroke(bool stroke)
{
    object_->setStyle(stroke ? Shape::kStroke : Shape::kFill);

}

void LuaShape::setStrokeEffect(bool dash, size_t count,
    BonesScalar * interval,
    BonesScalar offset)
{
    object_->setStrokeEffect(dash ? Shape::kDash : Shape::kSolid,
                             interval, count, offset);
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

}
