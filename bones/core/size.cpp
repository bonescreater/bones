#include "size.h"

namespace bones
{

Size Size::Make(float w, float h)
{
    return Size(w, h);
}

Size::Size()
{
    set(0, 0);
}

Size::Size(float w, float h)
{
    set(w, h);
}

void Size::set(float w, float h)
{
    width_ = w;
    height_ = h;
}

float Size::width() const
{
    return width_;
}

float Size::height() const
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