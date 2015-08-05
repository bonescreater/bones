#ifndef BONES_CORE_DEVICE_H_
#define BONES_CORE_DEVICE_H_

#include "core.h"

class SkBaseDevice;

namespace bones
{

class Pixmap;
class PixmapDevice;

class Device
{
public:
    static SkBaseDevice * Create(Pixmap & pm);

    static Pixmap & From(SkBaseDevice * sd);
};

}



#endif