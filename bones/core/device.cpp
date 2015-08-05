#include "device.h"
#include "SkBitmapDevice.h"
#include "pixmap.h"
#include "helper.h"

namespace bones
{

class PixmapDevice : public SkBitmapDevice
{
public:
    PixmapDevice(Pixmap & pm)
        :SkBitmapDevice(Helper::ToSkBitmap(pm)), target_(pm)
    {
        ;
    }

    Pixmap & getTarget()
    {
        return target_;
    }
private:
    Pixmap target_;
};


SkBaseDevice * Device::Create(Pixmap & pm)
{
    return new PixmapDevice(pm);
}

Pixmap & Device::From(SkBaseDevice * sd)
{
    return ((PixmapDevice *)sd)->getTarget();
}

}