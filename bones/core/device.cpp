#include "device.h"
#include "SkBitmapDevice.h"
#include "pixmap.h"
#include "helper.h"

namespace bones
{

class SurfaceDevice : public SkBitmapDevice
{
public:
    SurfaceDevice(Surface & pm)
        :SkBitmapDevice(Helper::ToSkBitmap(pm)), target_(pm)
    {
        ;
    }

    Surface & getTarget()
    {
        return target_;
    }
private:
    Surface target_;
};


SkBaseDevice * Device::Create(Surface & pm)
{
    return new SurfaceDevice(pm);
}

Surface & Device::From(SkBaseDevice * sd)
{
    return ((SurfaceDevice *)sd)->getTarget();
}

}