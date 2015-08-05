#ifndef BONES_CORE_PIXMAP_H_
#define BONES_CORE_PIXMAP_H_

#include "core.h"
#include "rect.h"
#include "color.h"

class SkPixelRef;

namespace bones
{
class Pixmap
{
public:
    struct LockRec
    {
        void * bits;
        size_t pitch;
        Rect subset;
    };
public:
    Pixmap();

    Pixmap(const Pixmap & pm);

    virtual ~Pixmap();

    bool decode(const void * buffer, size_t len);

    bool allocate(int width, int height, bool is_opaque = false);

    void free();

    bool lock(LockRec & rec);

    void unlock();

    bool isValid() const;

    bool isEmpty() const;

    int getWidth() const;

    int getHeight() const;

    void erase(Color color);
    //修改alpha 不会重新预乘颜色
    void negAlpha();

    Pixmap extractSubset(const Rect & subset);

    Pixmap & operator=(const Pixmap & pm);

    void copyTo(Pixmap & pm, int left, int top);
protected:
    virtual SkPixelRef * allocatePixelRef(int width, int height, bool is_opaque);
private:
    SkPixelRef * pixel_ref_;
    Rect subset_;
    friend class Helper;
};

class Surface : public Pixmap
{
public:
    Surface();
protected:
    SkPixelRef * allocatePixelRef(int width, int height, bool is_opaque) override;
private:
    static void PixelRefFree(void * addr, void * context);
private:
    HBITMAP hbm_;
    friend class Helper;
};

}
#endif