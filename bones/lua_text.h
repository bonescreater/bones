#ifndef BONES_LUA_TEXT_H_
#define BONES_LUA_TEXT_H_

#include "lua_object.h"

namespace bones
{
class Text;
class LuaText : public LuaObject<BonesText, BonesText::NotifyListener, Text>
{
public:
    LuaText(Text * ob);

    NotifyListener * getNotify() const override;

    void createMetaTable(lua_State * l) override;

    void setListener(NotifyListener * lis) override;
private:
    NotifyListener * notify_;
};

}


#endif