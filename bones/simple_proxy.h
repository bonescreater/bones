#ifndef BONES_SIMPLE_PROXY_H_
#define BONES_SIMPLE_PROXY_H_

#include "bones.h"

namespace bones
{

class PathProxy : public BonesPathProxy
{
public:
    BonesPath create() override;

    void moveTo(BonesPath path, const BonesPoint & p) override;

    void lineTo(BonesPath path, const BonesPoint & p) override;

    void quadTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2) override;

    void conicTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar w) override;

    void cubicTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, const BonesPoint & p3) override;

    void arcTo(BonesPath path, const BonesRect & oval,
        BonesScalar startAngle, BonesScalar sweepAngle) override;

    void arcTo(BonesPath path, const BonesPoint & p1,
        const BonesPoint & p2, BonesScalar radius) override;

    void close(BonesPath path) override;

    void release(BonesPath path) override;
};

class ShaderProxy : public BonesShaderProxy
{
public:
    BonesShader createLinearGradient(
        const BonesPoint & begin,
        const BonesPoint & end,
        TileMode mode,
        size_t count, BonesColor * color,
        BonesScalar * pos) override;

    BonesShader createRadialGradient(
        const BonesPoint & center,
        BonesScalar radius,
        TileMode mode,
        size_t count, BonesColor * color,
        float * pos) override;

    void release(BonesShader shader) override;
};

class PixmapProxy : public BonesPixmapProxy
{
public:
    BonesPixmap create() override;

    bool alloc(BonesPixmap pm, int width, int height) override;

    bool decode(BonesPixmap pm, const void * data, size_t len) override;

    int getWidth(BonesPixmap pm) const override;

    int getHeight(BonesPixmap pm) const override;

    bool lock(BonesPixmap pm, LockRec & rec) override;

    void unlock(BonesPixmap pm) override;

    void erase(BonesPixmap pm, BonesColor color) override;

    void extractSubset(BonesPixmap dst, 
        BonesPixmap src, const BonesRect & subset) override;

    bool isTransparent(BonesPixmap pm, int x, int y) override;

    void release(BonesPixmap pm) override;
};

class ResourceManager : public BonesResourceManager
{
public:
    void addPixmap(const char * key, BonesPixmap pm) override;

    void addCursor(const char * key, BonesCursor cursor) override;

    BonesPixmap getPixmap(const char * key) override;

    BonesCursor getCursor(const char * key) override;

    void clean() override;
};

}
#endif