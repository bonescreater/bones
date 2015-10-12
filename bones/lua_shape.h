#ifndef BONES_LUA_SHAPE_H_
#define BONES_LUA_SHAPE_H_

#include "lua_handler.h"
#include "core/shape.h"

namespace bones
{

class LuaShape : public LuaObject<BonesShape, Shape>,
                 public Shape::Delegate
{
public:
    LuaShape(Shape * ob);

    void createMetaTable(lua_State * l) override;

    void setStroke(bool stroke) override;

    void setStrokeEffect(size_t count,
        BonesScalar * interval,
        BonesScalar offset) override;

    void setStrokeWidth(BonesScalar stroke_width) override;

    void setColor(BonesColor color) override;

    void setColor(BonesShader shader) override;

    void setCircle(const BonesPoint & center, BonesScalar radius) override;

    void setRect(BonesScalar rx, BonesScalar ry,
                 const BonesRect * rect) override;

    void setLine(const BonesPoint & start, const BonesPoint & end) override;

    void setPoint(const BonesPoint & pt) override;

    void setOval(const BonesRect * oval) override;

    void setArc(BonesScalar start,
        BonesScalar sweep,
        bool use_center,
        const BonesRect * oval) override;

    LUA_HANDLER(Shape);
};

}

#endif