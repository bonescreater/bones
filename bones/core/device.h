#ifndef BONES_CORE_DEVICE_H_
#define BONES_CORE_DEVICE_H_

#include "core.h"

class SkBaseDevice;

namespace bones
{

class Surface;

class Device
{
public:
    static SkBaseDevice * Create(Surface & pm);

    static Surface & From(SkBaseDevice * sd);
};

}



#endif