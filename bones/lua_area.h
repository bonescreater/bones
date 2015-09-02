#ifndef BONES_LUA_AREA_H_
#define BONES_LUA_AREA_H_

#include "lua_object.h"
#include "core/area.h"

namespace bones
{

class LuaArea : public LuaObject<BonesArea, BonesArea::NotifyListener, Area>,
                public Area::Delegate
{
public:
    LuaArea(Area * ob);

    ~LuaArea();

    NotifyListener * getNotify() const override;

    void addEvent(const char * name,
        const char * phase,
        const char * module,
        const char * func);

    void setListener(MouseListener * lis) override;

    void setListener(KeyListener * lis) override;

    void setListener(FocusListener * lis) override;

    void setListener(WheelListener * lis) override;

    void setListener(NotifyListener * notify) override;

    void onMouseEnter(Area * sender, MouseEvent & e) override;

    void onMouseMove(Area * sender, MouseEvent & e) override;

    void onMouseDown(Area * sender, MouseEvent & e) override;

    void onMouseUp(Area * sender, MouseEvent & e) override;

    void onMouseClick(Area * sender, MouseEvent & e) override;

    void onMouseDClick(Area * sender, MouseEvent & e) override;

    void onMouseLeave(Area * sender, MouseEvent & e) override;

    void onKeyDown(Area * sender, KeyEvent & e) override;

    void onKeyUp(Area * sender, KeyEvent & e) override;

    void onKeyPress(Area * sender, KeyEvent & e) override;

    void onFocusOut(Area * sender, FocusEvent & e) override;

    void onFocusIn(Area * sender, FocusEvent & e) override;

    void onBlur(Area * sender, FocusEvent & e) override;

    void onFocus(Area * sender, FocusEvent & e) override;

    void onWheel(Area * sender, WheelEvent & e) override;

    void onSizeChanged(Area * sender, const Size & size) override;

    void onPositionChanged(Area * sender, const Point & loc) override;

    bool onHitTest(Area * sender, const Point & loc) override;
private:
    MouseListener * mouse_;
    KeyListener * key_;
    FocusListener * focus_;
    WheelListener * wheel_;
    NotifyListener * notify_;
};

}
#endif