#ifndef BONES_LUA_ROOT_H_
#define BONES_LUA_ROOT_H_

#include "lua_object.h"
#include "core/root.h"

namespace bones
{

class LuaRoot : public LuaObject<BonesRoot, BonesRoot::NotifyListener, Root>,
                public Root::Delegate
{
public:
    LuaRoot(Root *);

    ~LuaRoot();

    NotifyListener * getNotify() const override;

    void notifyCreate() override;

    void notifyDestroy() override;

    void createMetaTable(lua_State * l) override;

    void setListener(NotifyListener * listener) override;

    bool isDirty() const override;

    BonesRect getDirtyRect() const override;

    void draw() override;

    HBITMAP getBackBuffer() const override;

    void getBackBuffer(const void * & data, size_t & pitch) const;

    bool handleMouse(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleKey(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleFocus(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleComposition(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleWheel(UINT msg, WPARAM wparam, LPARAM lparam) override;

    void requestFocus(Root * sender) override;

    void invalidRect(Root * sender, const Rect & rect) override;

    void changeCursor(Root * sender, Cursor cursor) override;

    void createCaret(Root * sender, Caret caret, const Size & size) override;

    void showCaret(Root * sender, bool show) override;

    void changeCaretPos(Root * sender, const Point & pt) override;

    void onSizeChanged(Root * sender, const Size & size) override;

    void onPositionChanged(Root * sender, const Point & loc) override;
private:
    NotifyListener * listener_;
};


}


#endif