
#include "rect.h"

namespace bones
{

Rect Rect::MakeLTRB(Scalar left, Scalar top, Scalar right, Scalar bottom)
{
    return Rect(left, top, right, bottom);
}

Rect Rect::MakeXYWH(Scalar left, Scalar top, Scalar width, Scalar height)
{
    return Rect(left, top, left + width, top + height);
}

Rect::Rect()
{
    setEmpty();
}

Rect::Rect(Scalar left, Scalar top, Scalar right, Scalar bottom)
{
    setLTRB(left, top, right, bottom);
}

bool Rect::isEmpty() const
{
    return left_ >= right_ || top_ >= bottom_;
}

Scalar Rect::left() const
{
    return left_;
}

Scalar Rect::top() const
{
    return top_;
}

Scalar Rect::right() const
{
    return right_;
}

Scalar Rect::bottom() const
{
    return bottom_;
}

Scalar Rect::width() const
{
    return right_ - left_;
}

Scalar Rect::height() const
{
    return bottom_ - top_;
}

void Rect::setEmpty()
{
    memset(this, 0, sizeof(*this));
}

void Rect::setLTRB(Scalar left, Scalar top, Scalar right, Scalar bottom)
{
    left_ = left, top_ = top, right_ = right, bottom_ = bottom;
}

void Rect::setXYWH(Scalar left, Scalar top, Scalar width, Scalar height)
{
    setLTRB(left, top, left + width, top + height);
}

void Rect::offset(Scalar dx, Scalar dy)
{
    left_ += dx;
    top_ += dy;
    right_ += dx;
    bottom_ += dy;
}

void Rect::offsetTo(Scalar x, Scalar y)
{
    right_ += x - left_;
    bottom_ += y - top_;
    left_ = x;
    top_ = y;
}

void Rect::inset(Scalar dx, Scalar dy)
{
    left_ += dx;
    top_ += dy;
    right_ -= dx;
    bottom_ -= dy;
}

void Rect::outset(Scalar dx, Scalar dy)
{
    inset(-dx, -dy);
}

bool Rect::intersect(const Rect & r)
{
    Scalar left = r.left();
    Scalar top = r.top();
    Scalar right = r.right();
    Scalar bottom = r.bottom();

    if (!r.isEmpty() && !this->isEmpty() && // check for empties
        left_ < right && left < right_ && top_ < bottom && top < bottom_)
    {
        if (left_ < left) left_ = left;
        if (top_ < top) top_ = top;
        if (right_ > right) right_ = right;
        if (bottom_ > bottom) bottom_ = bottom;
        return true;
    }
    return false;
}


void Rect::join(const Rect & r)
{
    Scalar left = r.left();
    Scalar top = r.top();
    Scalar right = r.right();
    Scalar bottom = r.bottom();

    if (left >= right || top >= bottom) {
        return;
    }

    // if we are empty, just assign
    if (this->isEmpty()) 
    {
        this->setLTRB(left, top, right, bottom);
    }
    else 
    {
        if (left < left_) left_ = left;
        if (top < top_) top_ = top;
        if (right > right_) right_ = right;
        if (bottom > bottom_) bottom_ = bottom;
    }
}

bool Rect::contains(Scalar x, Scalar y)
{
    return !this->isEmpty() &&
        left_ <= x && x < right_ && top_ <= y && y < bottom_;
}


}