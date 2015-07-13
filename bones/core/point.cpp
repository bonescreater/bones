#include "point.h"

namespace bones
{

Point Point::Make(Scalar x, Scalar y)
{
     return Point(x, y);
}

Point::Point()
{
    set(0, 0);
}

Point::Point(Scalar x, Scalar y)
{
    set(x, y);
}

void Point::set(Scalar x, Scalar y)
{
    x_ = x, y_ = y;
}

Scalar Point::x() const
{
    return x_;
}

Scalar Point::y() const
{
    return y_;
}

bool Point::isZero() const
{
    return 0 == x_ && 0 == y_;
}

bool Point::operator == (const Point & rh)
{
    return x_ == rh.x_ && y_ == rh.y_;
}

bool Point::operator != (const Point & rh)
{
    return !(*this == rh);
}

}