#ifndef BONES_LUA_IMAGE_H_
#define BONES_LUA_IMAGE_H_

#include "lua_handler.h"
#include "core/image.h"

namespace bones
{

class LuaImage : public LuaObject<BonesImage, Image>,
                 public Image::Delegate
{
public:
    LuaImage(Image * ob);

    void createMetaTable(lua_State * l) override;

    void setDirect(const BonesPoint * bp) override;

    void setStretch(const BonesRect * dst) override;

    void setNine(const BonesRect * dst, const BonesRect * center) override;

    void setContent(const BonesPixmap & pm) override;

    void setContent(const char * key) override;

    BonesPMColor getPMColor(int x, int y) override;

    void setColorMatrix(const BonesColorMatrix * cm) override;

    LUA_HANDLER(Image);
};

}

#endif