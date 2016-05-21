#ifndef BONES_BINDING_PATH_PROXY_H_
#define BONES_BINDING_PATH_PROXY_H_

#include "bones.h"
#include "lua.hpp"

namespace bones
{

class PathProxy : public BonesPathProxy
{
public:
    PathProxy(lua_State * s);

    ~PathProxy();
public:
    //pathproxy
    BonesPath create() override;

    void moveTo(BonesPath path, const BonesPoint & p) override;

    void lineTo(BonesPath path, const BonesPoint & p) override;

    void quadTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2) override;

    void conicTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar w) override;

    void cubicTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, const BonesPoint & p3) override;

    void arcTo(BonesPath path, const BonesRect & oval,
        BonesScalar startAngle, BonesScalar sweepAngle) override;

    void arcTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar radius) override;

    void close(BonesPath path) override;

    void release(BonesPath path) override;
private:
    lua_State * state_;
};

}

#endif