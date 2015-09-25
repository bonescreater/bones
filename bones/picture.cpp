#include "picture.h"

namespace bones
{


Picture::Picture()
{
    ;
}

Picture::~Picture()
{
    ;
}

Pixmap & Picture::getPixmap()
{
    return pixmap_;
}

const Pixmap & Picture::getPixmap() const
{
    return pixmap_;
}

bool Picture::alloc(int width, int height)
{
    return pixmap_.allocate(width, height);
}

bool Picture::decode(const void * data, size_t len)
{
    return pixmap_.decode(data, len);
}

int Picture::getWidth() const
{
    return pixmap_.getWidth();
}

int Picture::getHeight() const
{
    return pixmap_.getHeight();
}

bool Picture::lock(LockRec & rec)
{
    Pixmap::LockRec pm_rec;
    bool bret = pixmap_.lock(pm_rec);

    if (bret)
    {
        rec.bits = pm_rec.bits;
        rec.pitch = pm_rec.pitch;
        rec.subset.left = pm_rec.subset.left();
        rec.subset.top = pm_rec.subset.top();
        rec.subset.right = pm_rec.subset.right();
        rec.subset.bottom = pm_rec.subset.bottom();
    }
    return bret;
}

void Picture::unlock()
{
    return pixmap_.unlock();
}

void Picture::erase(BonesColor color)
{
    return pixmap_.erase(color);
}

void Picture::extractSubset(BonesPixmap & pm, const BonesRect & subset)
{
    static_cast<Picture *>(&pm)->pixmap_ = pixmap_.extractSubset(
        Rect::MakeLTRB(subset.left, subset.top, subset.right, subset.bottom));
}



}