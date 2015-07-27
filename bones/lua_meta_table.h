#ifndef BONES_LUA_META_TABLE_H_
#define BONES_LUA_META_TABLE_H_

#include "bones.h"

namespace bones
{

class LuaMetaTable
{
public:
    static void GetPanel(lua_State * l);

    static void GetArea(lua_State * l);

    static void GetBlock(lua_State * l);

    static void GetText(lua_State * l);

    static void GetShape(lua_State * l);

    static void GetMouseEvent(lua_State * l);

    static void GetFocusEvent(lua_State * l);

    static void GetAnimation(lua_State * l);

    static void SetClosureCObject(lua_State * l, Ref * ref);

    static Ref * CallGetCObject(lua_State *l);
protected:
    static void GetRef(lua_State *l, const char * class_name);

    static void GetView(lua_State * l, const char * class_name);

    static void GetEvent(lua_State * l, const char * class_name);

    static void GetUIEvent(lua_State * l, const char * class_name);
};

}
#endif