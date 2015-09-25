#ifndef BONES_LUA_SHAPE_H_
#define BONES_LUA_SHAPE_H_

#include "lua_handler.h"
#include "core/shape.h"

namespace bones
{

class LuaShape : public LuaObject<BonesShape, Shape>,
                 public Shape::Delegate
{
public:
    LuaShape(Shape * ob);

    void createMetaTable(lua_State * l) override;

    LUA_HANDLER(Shape);
//    void notifyCreate();
//
//    void notifyDestroy();
//
//    void createMetaTable(lua_State * l) override;
//
//    void setListener(NotifyListener * lis) override;
//
//    void setListener(BonesEvent::Phase phase, MouseListener * lis) override;
//
//    void setListener(BonesEvent::Phase phase, KeyListener * lis) override;
//
//    void setListener(BonesEvent::Phase phase, FocusListener * lis) override;
//
//    void setListener(BonesEvent::Phase phase, WheelListener * lis) override;
//
//    void onMouseEnter(Shape * sender, MouseEvent & e)  override;
//
//    void onMouseMove(Shape * sender, MouseEvent & e) override;
//
//    void onMouseDown(Shape * sender, MouseEvent & e) override;
//
//    void onMouseUp(Shape * sender, MouseEvent & e) override;
//
//    void onMouseClick(Shape * sender, MouseEvent & e)  override;
//
//    void onMouseDClick(Shape * sender, MouseEvent & e) override;
//
//    void onMouseLeave(Shape * sender, MouseEvent & e)  override;
//
//    void onKeyDown(Shape * sender, KeyEvent & e) override;
//
//    void onKeyUp(Shape * sender, KeyEvent & e) override;
//
//    void onChar(Shape * sender, KeyEvent & e) override;
//
//    void onFocusOut(Shape * sender, FocusEvent & e) override;
//
//    void onFocusIn(Shape * sender, FocusEvent & e) override;
//
//    void onBlur(Shape * sender, FocusEvent & e) override;
//
//    void onFocus(Shape * sender, FocusEvent & e) override;
//
//    void onWheel(Shape * sender, WheelEvent & e) override;
//
//    void onSizeChanged(Shape * sender, const Size & size) override;
//
//    void onPositionChanged(Shape * sender, const Point & loc) override;
//
//    bool onHitTest(Shape * sender, const Point & loc) override;
//protected:
//    NotifyListener * notify_;
//    MouseListener * mouse_[BonesEvent::kPhaseCount];
//    KeyListener * key_[BonesEvent::kPhaseCount];
//    FocusListener * focus_[BonesEvent::kPhaseCount];
//    WheelListener * wheel_[BonesEvent::kPhaseCount];
};

}

#endif