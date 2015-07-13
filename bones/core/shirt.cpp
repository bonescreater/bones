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

bool Shirt::isFocusable() const
{
    return false;
}


}