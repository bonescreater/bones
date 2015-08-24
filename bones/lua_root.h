#ifndef BONES_LUA_ROOT_H_
#define BONES_LUA_ROOT_H_

#include "lua_object.h"
#include "core/root.h"

namespace bones
{

class LuaRoot : public LuaObject<BonesRoot, Root>, 
                public Root::Delegate
{
public:
    LuaRoot(Root *);

    void addNotify(const char * notify_name, const char * mod, const char * func);

    void setListener(Listener * listener) override;

    bool isDirty() const override;

    BonesRect getDirtyRect() const override;

    void draw() override;

    HDC dc() const override;

    void requestFocus(Ref * sender) override;

    void invalidRect(Ref * sender, const Rect & rect) override;

    void changeCursor(Ref * sender, Cursor cursor) override;

    void onSizeChanged(Ref * sender, const Size & size) override;
private:
    Listener * listener_;
};


}


#endif