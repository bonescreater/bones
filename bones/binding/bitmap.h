#ifndef BONES_BINDING_BITMAP_H_
#define BONES_BINDING_BITMAP_H_

#include "bones.h"
#include "engine_context.h"


class SkBitmap;

namespace bones
{

class Bitmap : public BonesPixmap
{
public:
    Bitmap(EngineContext & ctx);

    virtual ~Bitmap();

    bool alloc(const BonesISize & size) override;

    bool decode(const void * data, size_t len) override;

    int getWidth() const override;

    int getHeight() const override;

    void lock() override;

    void unlock() override;

    void erase(BonesColor color) override;

    bool extractSubset(BonesPixmap & dst, const BonesIRect & subset) override;

    bool isTransparent(int x, int y) override;

    bool isValid()   const override;
public:
    static Bitmap * CreateLuaNewUserData(lua_State * l, EngineContext & ctx);

    static Bitmap * Cast(lua_State *l, int idx);
public:
    bool invokeAlloc(const BonesISize & size);

    bool invokeDecode(const void * data, size_t len);

    int invokeGetWidth() const;

    int invokeGetHeight() const;

    void invokeLock();

    void invokeUnlock();

    void invokeErase(BonesColor color);

    bool invokeExtractSubset(BonesPixmap &dst, const BonesIRect & subset);

    bool invokeIsTransparent(int x, int y);

    bool invokeIsValid() const;

    SkBitmap & GetBitmap();

    Bitmap & operator = (const Bitmap & right);
private:
    EngineContext * ctx_;
    SkBitmap * bitmap_;
};

}

#endif