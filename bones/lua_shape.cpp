#include "lua_shape.h"
#include "lua_context.h"
#include "lua_check.h"

#include "core/logging.h"

namespace bones
{
static const char * kMetaTableShape = "__mt_shape";

LuaShape::LuaShape(Shape * co)
:LuaObject(co), notify_(nullptr)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

void LuaShape::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableShape);
}

//void LuaShape::notifyCreate()
//{
//    object_->setDelegate(this);
//    bool stop = false;
//    notify_ ? notify_->onCreate(this, stop) : 0;
//    if (stop)
//        return;
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodOnCreate);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        auto count = LuaContext::SafeLOPCall(l, 1, 0);
//        lua_pop(l, count);
//    }
//    lua_pop(l, 2);
//}

//void LuaShape::notifyDestroy()
//{
//    bool stop = false;
//    notify_ ? notify_->onDestroy(this, stop) : 0;
//    if (stop)
//        return;
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodOnDestroy);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        auto count = LuaContext::SafeLOPCall(l, 1, 0);
//        lua_pop(l, count);
//    }
//    lua_pop(l, 2);
//    object_->setDelegate(nullptr);
//}

//void LuaShape::setListener(NotifyListener * lis) 
//{
//    notify_ = lis;
//}


//void LuaShape::setListener(BonesEvent::Phase phase, MouseListener * lis)
//{
//    if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)
//        return;
//    mouse_[phase] = lis;
//}

//void LuaShape::setListener(BonesEvent::Phase phase, KeyListener * lis)
//{
//    if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)
//        return;
//    key_[phase] = lis;
//}
//
//void LuaShape::setListener(BonesEvent::Phase phase, FocusListener * lis)
//{
//    if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)
//        return;
//    focus_[phase] = lis;
//}
//
//void LuaShape::setListener(BonesEvent::Phase phase, WheelListener * lis)
//{
//    if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)
//        return;
//    wheel_[phase] = lis;
//}

//template<class T>
//static void PostToScript(LuaShape * ob, T & e, const char * type)
//{
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, ob);
//    lua_getfield(l, -1, kEventOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, ToEventPhaseStr(e.phase()));
//        if (lua_istable(l, -1))
//        {
//            lua_getfield(l, -1, type);
//            lua_pushnil(l);
//            lua_copy(l, -5, -1);
//            void ** ud = (void **)LuaContext::GetEventCache(l,
//                EventStack().getCount());
//            *ud = &e;
//            T::GetMetaTable(l);
//            lua_setmetatable(l, -2);
//            LuaContext::SafeLOPCall(l, 2, 0);
//        }
//        lua_pop(l, 1);
//    }
//    lua_pop(l, 2);
//}

//void LuaShape::onMouseEnter(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseEnter(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseEnter);
//}

//void LuaShape::onMouseMove(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseMove(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseMove);
//}

//void LuaShape::onMouseDown(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseDown(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseDown);
//}

//void LuaShape::onMouseUp(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseUp(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseUp);
//}

//void LuaShape::onMouseClick(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseClick(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseClick);
//}

//void LuaShape::onMouseDClick(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseDClick(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseDClick);
//}

//void LuaShape::onMouseLeave(Shape * sender, MouseEvent & e)
//{
//    bool stop = false;
//    LuaMouseEvent le(e);
//    mouse_[le.phase()] ? mouse_[le.phase()]->onMouseLeave(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnMouseLeave);
//}

//void LuaShape::onKeyDown(Shape * sender, KeyEvent & e)
//{
//    bool stop = false;
//    LuaKeyEvent le(e);
//    key_[le.phase()] ? key_[le.phase()]->onKeyDown(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnKeyDown);
//}

//void LuaShape::onKeyUp(Shape * sender, KeyEvent & e)
//{
//    bool stop = false;
//    LuaKeyEvent le(e);
//    key_[le.phase()] ? key_[le.phase()]->onKeyUp(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnKeyUp);
//}

//void LuaShape::onChar(Shape * sender, KeyEvent & e)
//{
//    bool stop = false;
//    LuaKeyEvent le(e);
//    key_[le.phase()] ? key_[le.phase()]->onChar(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnChar);
//}

//void LuaShape::onFocusOut(Shape * sender, FocusEvent & e)
//{
//    bool stop = false;
//    LuaFocusEvent le(e);
//    focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnFocusOut);
//}

//void LuaShape::onFocusIn(Shape * sender, FocusEvent & e)
//{
//    bool stop = false;
//    LuaFocusEvent le(e);
//    focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnFocusIn);
//}

//void LuaShape::onBlur(Shape * sender, FocusEvent & e)
//{
//    bool stop = false;
//    LuaFocusEvent le(e);
//    focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnBlur);
//}

//void LuaShape::onFocus(Shape * sender, FocusEvent & e)
//{
//    bool stop = false;
//    LuaFocusEvent le(e);
//    focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnFocus);
//}

//void LuaShape::onWheel(Shape * sender, WheelEvent & e)
//{
//    bool stop = false;
//    LuaWheelEvent le(e);
//    wheel_[le.phase()] ? wheel_[le.phase()]->onWheel(this, le, stop) : 0;
//    if (stop)
//        return;
//    PostToScript(this, le, kMethodOnWheel);
//}

//void LuaShape::onSizeChanged(Shape * sender, const Size & size)
//{
//    bool stop = false;
//    BonesSize bs = { size.width(), size.height() };
//    notify_ ? notify_->onSizeChanged(this, bs, stop) : 0;
//    if (stop)
//        return;
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodOnSizeChanged);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        lua_pushnumber(l, bs.width);
//        lua_pushnumber(l, bs.height);
//        LuaContext::SafeLOPCall(l, 3, 0);
//    }
//    lua_pop(l, 2);
//}
//
//void LuaShape::onPositionChanged(Shape * sender, const Point & loc)
//{
//    bool stop = false;
//    BonesPoint bp = { loc.x(), loc.y() };
//    notify_ ? notify_->onPositionChanged(this, bp, stop) : 0;
//    if (stop)
//        return;
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodOnPositionChanged);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        lua_pushnumber(l, bp.x);
//        lua_pushnumber(l, bp.y);
//        LuaContext::SafeLOPCall(l, 3, 0);
//    }
//    lua_pop(l, 2);
//}
//
//bool LuaShape::onHitTest(Shape * sender, const Point & loc)
//{
//    bool stop = false;
//    BonesPoint bp = { loc.x(), loc.y() };
//    bool ht = notify_ ? notify_->onHitTest(this, bp, stop) : true;
//    if (stop)
//        return ht;
//
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodOnHitTest);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        lua_pushnumber(l, bp.x);
//        lua_pushnumber(l, bp.y);
//        auto count = LuaContext::SafeLOPCall(l, 3, 1);
//        if (count > 0)
//            ht = !!lua_toboolean(l, -1);
//        lua_pop(l, count);
//    }
//    lua_pop(l, 2);
//    return ht;
//}


}
