#ifndef BONES_LUA_RICH_EDIT_H_
#define BONES_LUA_RICH_EDIT_H_

#include "lua_object.h"

namespace bones
{

class RichEdit;

class LuaRichEdit : public LuaObject<BonesRichEdit, RichEdit>
{
public:
    LuaRichEdit(RichEdit * ob);
};


}


#endif