#ifndef BONES_CORE_RECT_H_
#define BONES_CORE_RECT_H_

#include "core.h"

namespace bones
{

class Rect
{
public:
    static Rect MakeLTRB(Scalar left, Scalar top, Scalar right, Scalar bottom);

    static Rect MakeXYWH(Scalar left, Scalar top, Scalar width, Scalar height);

    Rect();

    Rect(Scalar left, Scalar top, Scalar right, Scalar bottom);

    bool isEmpty() const;

    Scalar left() const;

    Scalar top() const;

    Scalar right() const;

    Scalar bottom() const;

    Scalar width() const;

    Scalar height() const;

    void setEmpty();

    void setLTRB(Scalar left, Scalar top, Scalar right, Scalar bottom);

    void setXYWH(Scalar left, Scalar top, Scalar width, Scalar height);

    void offset(Scalar dx, Scalar dy);

    void offsetTo(Scalar x, Scalar y);

    void inset(Scalar dx, Scalar dy);

    void outset(Scalar dx, Scalar dy);

    bool intersect(const Rect & r);

    void join(const Rect & r);

    bool contains(Scalar x, Scalar y);
private:
    Scalar left_;
    Scalar top_;
    Scalar right_;
    Scalar bottom_;
};


}
#endif