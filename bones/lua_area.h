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

    void notifyCreate() override;

    void notifyDestroy() override;

    void createMetaTable(lua_State * l) override;

    void addEvent(const char * name,
        const char * phase,
        const char * module,
        const char * func);

    void setListener(BonesEvent::Phase phase, MouseListener * lis) override;

    void setListener(BonesEvent::Phase phase, KeyListener * lis) override;

    void setListener(BonesEvent::Phase phase, FocusListener * lis) override;

    void setListener(BonesEvent::Phase phase, WheelListener * lis) override;

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

    void onChar(Area * sender, KeyEvent & e) override;

    void onFocusOut(Area * sender, FocusEvent & e) override;

    void onFocusIn(Area * sender, FocusEvent & e) override;

    void onBlur(Area * sender, FocusEvent & e) override;

    void onFocus(Area * sender, FocusEvent & e) override;

    void onWheel(Area * sender, WheelEvent & e) override;

    void onSizeChanged(Area * sender, const Size & size) override;

    void onPositionChanged(Area * sender, const Point & loc) override;

    bool onHitTest(Area * sender, const Point & loc) override;
private:
    MouseListener * mouse_[BonesEvent::kPhaseCount];
    KeyListener * key_[BonesEvent::kPhaseCount];
    FocusListener * focus_[BonesEvent::kPhaseCount];
    WheelListener * wheel_[BonesEvent::kPhaseCount];
    NotifyListener * notify_;
};

}
#endif