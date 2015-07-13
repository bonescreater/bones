#ifndef BONES_CORE_POINT_H_
#define BONES_CORE_POINT_H_

#include "core.h"

namespace bones
{
class Point
{
public:
    static Point Make(Scalar x, Scalar y);

    Point();

    Point(Scalar x, Scalar y);

    void set(Scalar x, Scalar y);

    Scalar x() const;

    Scalar y() const;

    bool isZero() const;

    bool operator == (const Point & rh);
    
    bool operator != (const Point & rh);
private:
    Scalar x_;
    Scalar y_;
};
}
#endif