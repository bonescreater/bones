#ifndef BONES_PICTURE_H_
#define BONES_PICTURE_H_

#include "bones_internal.h"
#include "core/pixmap.h"
#include "core/ref.h"

namespace bones
{

class Picture : public BonesPixmap, public Ref
{
public:
    Picture();

    ~Picture();

    Pixmap & getPixmap();

    void retain() override;

    void release() override;

    bool alloc(int width, int height) override;

    bool decode(const void * data, size_t len) override;

    int getWidth() const override;

    int getHeight() const override;

    bool lock(LockRec & rec) override;

    void unlock() override;

    void erase(BonesColor color) override;

    void extractSubset(BonesPixmap & pm, const BonesRect & subset) override;
private:
    Pixmap pixmap_;
};

}

#endif