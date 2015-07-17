#include "shirt.h"

namespace bones
{


Shirt::Shirt()
:opacity_(1.0f)
{
    ;
}

void Shirt::setOpacity(float opacity)
{
    opacity_ = opacity;
    inval();
}

float Shirt::getOpacity() const
{
    return opacity_;
}

bool Shirt::isFocusable() const
{
    return false;
}


}