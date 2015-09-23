#ifndef BONES_LUA_RICH_EDIT_H_
#define BONES_LUA_RICH_EDIT_H_

#include "lua_object.h"
#include "core/rich_edit.h"

namespace bones
{

class LuaRichEdit : public LuaObject<BonesRichEdit, BonesRichEdit::NotifyListener, RichEdit>,
                    public RichEdit::Delegate
{
public:
    LuaRichEdit(RichEdit * ob);

    ~LuaRichEdit();

    NotifyListener * getNotify() const override;

    void notifyCreate() override;

    void notifyDestroy() override;

    void createMetaTable(lua_State * l) override;

    void setListener(NotifyListener * listener) override;

    HRESULT txNotify(RichEdit * sender, DWORD iNotify, void  *pv) override;
private:
    NotifyListener * listener_;
};


}


#endif