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

    void getBackBuffer(const void * & data, size_t & pitch) const;

    bool handleMouse(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleKey(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleFocus(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleComposition(UINT msg, WPARAM wparam, LPARAM lparam) override;

    bool handleWheel(UINT msg, WPARAM wparam, LPARAM lparam) override;

    void requestFocus(Root * sender) override;

    void invalidRect(Root * sender, const Rect & rect) override;

    void changeCursor(Root * sender, Cursor cursor) override;

    LUA_HANDLER(Root);
};


}


#endif