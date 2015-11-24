#ifndef BONES_LUA_ROOT_H_
#define BONES_LUA_ROOT_H_

#include "lua_handler.h"
#include "core/root.h"

namespace bones
{

class LuaRoot : public LuaObject<BonesRoot, Root>,
                public Root::Delegate
{
public:
    LuaRoot(Root *);

    ~LuaRoot();

    void createMetaTable(lua_State * l) override;

    void setColor(BonesColor color) override;

    void attachTo(BonesWidget hwnd) override;

    bool isDirty() const override;

    BonesRect getDirtyRect() const override;

    void draw() override;

    HBITMAP getBackBuffer() const override;

    void getBackBuffer(const void * & data, size_t & pitch) const override;

    void sendMouse(MouseMessage msg, const BonesPoint & pt, int flags) override;

    void sendKey(KeyMessage msg, int32_t vk, uint32_t states, int flags) override;

    void sendWheel(int dx, int dy, const BonesPoint & pt, int flags) override;

    void sendFocus(bool focus) override;

    void sendComposition(IMEMessage msg, const IMEInfo * info) override;

    void requestFocus(Root * sender) override;

    void shiftFocus(Root * sender, View * prev, View * current) override;

    void changeCaretPos(Root * sender, const Point & pt) override;

    void invalidRect(Root * sender, const Rect & rect) override;

    void changeCursor(Root * sender, View::Cursor cursor, void * content) override;

    LUA_HANDLER(Root);
};


}


#endif