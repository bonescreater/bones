#ifndef BONES_BINDING_PATH_PROXY_H_
#define BONES_BINDING_PATH_PROXY_H_

#include "core/core.h"
#include "lua.hpp"

namespace bones
{

class PathProxy
{
    typedef void * Path;
public:
    PathProxy(lua_State * s);

    ~PathProxy();

    Path create();

    void moveTo(Path path, Scalar px, Scalar py);

    void lineTo(Path path, Scalar px, Scalar py);

    void quadTo(Path path, Scalar p1x, Scalar p1y,
                Scalar p2x, Scalar p2y);

    void conicTo(Path path, Scalar p1x, Scalar p1y,
        Scalar p2x, Scalar p2y, Scalar w);

    void cubicTo(Path path, Scalar p1x, Scalar p1y,
        Scalar p2x, Scalar p2y, Scalar p3x, Scalar p3y);

    void arcTo(Path path, Scalar l, Scalar t, Scalar r, Scalar b,
        Scalar startAngle, Scalar sweepAngle);

    void arcTo(Path path, Scalar p1x, Scalar p1y,
        Scalar p2x, Scalar p2y, Scalar radius);

    void close(Path path);

    void release(Path path);
private:
    lua_State * state_;
};

}

#endif