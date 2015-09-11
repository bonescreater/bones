#ifndef BONES_LUA_IMAGE_H_
#define BONES_LUA_IMAGE_H_

#include "lua_object.h"

namespace bones
{
class Image;
class LuaImage : public LuaObject<BonesImage, BonesImage::NotifyListener, Image>
{
public:
    LuaImage(Image * ob);

    NotifyListener * getNotify() const override;

    void createMetaTable(lua_State * l) override;

    void setListener(NotifyListener * lis) override;
private:
    NotifyListener * notify_;
};

}

#endif