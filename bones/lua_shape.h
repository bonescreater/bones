#ifndef BONES_LUA_SHAPE_H_
#define BONES_LUA_SHAPE_H_

#include "lua_object.h"

namespace bones
{

class Shape;
class LuaShape : public LuaObject<BonesShape, BonesShape::NotifyListener, Shape>
{
public:
    LuaShape(Shape * ob);

    NotifyListener * getNotify() const override;

    void setListener(NotifyListener * lis) override;
private:
    NotifyListener * notify_;
};

}

#endif