#ifndef BONES_LUA_RICH_EDIT_H_
#define BONES_LUA_RICH_EDIT_H_

#include "lua_object.h"
#include "core/rich_edit.h"

namespace bones
{

class LuaRichEdit : public LuaObject<BonesRichEdit, RichEdit>,
                    public RichEdit::Delegate
{
public:
    LuaRichEdit(RichEdit * ob);

    ~LuaRichEdit();

    void setListener(Listener * listener) override;

    BOOL screenToClient(RichEdit * sender, LPPOINT lppt) override;

    BOOL clientToScreen(RichEdit * sender, LPPOINT lppt) override;

    HIMC immGetContext(RichEdit * sender) override;

    void immReleaseContext(RichEdit * sender, HIMC himc) override;

    HRESULT txNotify(RichEdit * sender, DWORD iNotify, void  *pv) override;

    void onReturn(RichEdit * sender) override;
private:
    Listener * listener_;
};


}


#endif