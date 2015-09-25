#ifndef BONES_LUA_RICH_EDIT_H_
#define BONES_LUA_RICH_EDIT_H_

#include "lua_handler.h"
#include "core/rich_edit.h"

namespace bones
{

class LuaRichEdit : public LuaObject<BonesRichEdit, RichEdit>,
                    public RichEdit::Delegate
{
public:
    LuaRichEdit(RichEdit * ob);

    void createMetaTable(lua_State * l) override;

    HRESULT txNotify(RichEdit * sender, DWORD iNotify, void  *pv) override;

    LUA_HANDLER(RichEdit);
};


}


#endif