#include "shirt.h"

namespace bones
{


Shirt::Shirt()
:opacity_(1.0f)
{
    setFocusable(false);
}

void Shirt::setOpacity(float opacity)
{
    if (opacity_ != opacity)
    {
        opacity_ = opacity;
        inval();
    }
}

float Shirt::getOpacity() const
{
    return opacity_;
}


}