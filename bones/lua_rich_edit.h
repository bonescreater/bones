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

    void killTimer(Ref * sender, UINT idTimer) override;

    BOOL setTimer(Ref * sender, UINT idTimer, UINT uTimeout) override;

    BOOL showCaret(Ref * sender, BOOL fshow) override;

    BOOL createCaret(Ref * sender, HBITMAP hbmp, INT xWidth, INT yHeight) override;

    BOOL setCaretPos(Ref * sender, INT x, INT y) override;

    BOOL screenToClient(Ref * sender, LPPOINT lppt) override;

    BOOL clientToScreen(Ref * sender, LPPOINT lppt) override;

    HIMC immGetContext(Ref * sender) override;

    void immReleaseContext(Ref * sender, HIMC himc) override;
private:
    Listener * listener_;
};


}


#endif