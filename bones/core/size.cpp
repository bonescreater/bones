#include "size.h"

namespace bones
{

Size Size::Make(Scalar w, Scalar h)
{
    return Size(w, h);
}

Size::Size()
{
    set(0, 0);
}

Size::Size(Scalar w, Scalar h)
{
    set(w, h);
}

void Size::set(Scalar w, Scalar h)
{
    width_ = w;
    height_ = h;
}

Scalar Size::width() const
{
    return width_;
}

Scalar Size::height() const
{
    return height_;
}

bool Size::operator == (const Size & rh)
{
    return width_ == rh.width_ && height_ == rh.height_;
}

bool Size::operator != (const Size & rh)
{
    return !(*this == rh);
}


}