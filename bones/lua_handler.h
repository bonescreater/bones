#ifndef BONES_LUA_HANDLER_H_
#define BONES_LUA_HANDLER_H_

#include "lua_object.h"
#include "lua_event.h"

namespace bones
{


template<class E >
void PostEventToScript(void * ob, E & e, const char * type)
{
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, ob);
    lua_getfield(l, -1, kEventOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, ToEventPhaseStr(e.phase()));
        if (lua_istable(l, -1))
        {
            lua_getfield(l, -1, type);
            lua_pushnil(l);
            lua_copy(l, -5, -1);
            void ** ud = (void **)LuaContext::GetEventCache(l,
                EventStack().getCount());
            *ud = &e;
            E::GetMetaTable(l);
            lua_setmetatable(l, -2);
            LuaContext::SafeLOPCall(l, 2, 0);
        }
        lua_pop(l, 1);
    }
    lua_pop(l, 2);
}

#define LUA_HANDLER(T) \
public:\
    void notifyCreate()\
    {\
    \
        object_->setDelegate(this);\
        bool stop = false;\
        notify_ ? notify_->onCreate(this, stop) : 0;\
        if (stop)\
            return;\
        auto l = LuaContext::State();\
        LUA_STACK_AUTO_CHECK(l);\
        LuaContext::GetLOFromCO(l, this);\
        lua_getfield(l, -1, kNotifyOrder);\
        if (lua_istable(l, -1))\
        {\
            lua_getfield(l, -1, kMethodOnCreate);\
            lua_pushnil(l);\
            lua_copy(l, -4, -1);\
            auto count = LuaContext::SafeLOPCall(l, 1, 0);\
            lua_pop(l, count);\
        }\
        lua_pop(l, 2);\
    }\
    void notifyDestroy()\
    {\
        bool stop = false;\
        notify_ ? notify_->onDestroy(this, stop) : 0;\
        if (stop)\
            return;\
        auto l = LuaContext::State();\
        LUA_STACK_AUTO_CHECK(l);\
        LuaContext::GetLOFromCO(l, this);\
        lua_getfield(l, -1, kNotifyOrder);\
        if (lua_istable(l, -1))\
        {\
            lua_getfield(l, -1, kMethodOnDestroy);\
            lua_pushnil(l);\
            lua_copy(l, -4, -1);\
            auto count = LuaContext::SafeLOPCall(l, 1, 0);\
            lua_pop(l, count);\
        }\
        lua_pop(l, 2);\
        object_->setDelegate(nullptr);\
    }\
    \
    void setListener(NotifyListener * lis) override\
    {\
        notify_ = lis;\
    }\
    void setListener(BonesEvent::Phase phase, MouseListener * lis) override\
    {\
        if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)\
            return;\
        mouse_[phase] = lis;\
    }\
    void setListener(BonesEvent::Phase phase, KeyListener * lis) override\
    {\
        if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)\
            return;\
        key_[phase] = lis;\
    }\
    void setListener(BonesEvent::Phase phase, FocusListener * lis) override\
    {\
        if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)\
            return;\
        focus_[phase] = lis;\
    }\
    void setListener(BonesEvent::Phase phase, WheelListener * lis) override\
    {\
        if (phase >= BonesEvent::kPhaseCount || phase < BonesEvent::kCapture)\
            return;\
        wheel_[phase] = lis;\
    }\
    void onMouseEnter(T * sender, MouseEvent & e)  override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseEnter(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseEnter);\
    }\
    void onMouseMove(T * sender, MouseEvent & e) override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseMove(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseMove);\
    }\
    void onMouseDown(T * sender, MouseEvent & e) override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseDown(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseDown);\
    }\
    void onMouseUp(T * sender, MouseEvent & e) override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseUp(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseUp);\
    }\
    void onMouseClick(T * sender, MouseEvent & e)  override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseClick(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseClick);\
    }\
    void onMouseDClick(T * sender, MouseEvent & e) override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseDClick(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseDClick);\
    }\
    void onMouseLeave(T * sender, MouseEvent & e)  override\
    {\
        bool stop = false;\
        LuaMouseEvent le(e);\
        mouse_[le.phase()] ? mouse_[le.phase()]->onMouseLeave(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnMouseLeave);\
    }\
    void onKeyDown(T * sender, KeyEvent & e) override\
    {\
        bool stop = false;\
        LuaKeyEvent le(e);\
        key_[le.phase()] ? key_[le.phase()]->onKeyDown(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnKeyDown);\
    }\
    void onKeyUp(T * sender, KeyEvent & e) override\
    {\
        bool stop = false;\
        LuaKeyEvent le(e);\
        key_[le.phase()] ? key_[le.phase()]->onKeyUp(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnKeyUp);\
    }\
    void onChar(T * sender, KeyEvent & e) override\
    {\
        bool stop = false;\
        LuaKeyEvent le(e);\
        key_[le.phase()] ? key_[le.phase()]->onChar(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnChar);\
    }\
    void onFocusOut(T * sender, FocusEvent & e) override\
    {\
        bool stop = false;\
        LuaFocusEvent le(e);\
        focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnFocusOut);\
    }\
    void onFocusIn(T * sender, FocusEvent & e) override\
    {\
        bool stop = false;\
        LuaFocusEvent le(e);\
        focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnFocusIn);\
    }\
    void onBlur(T * sender, FocusEvent & e) override\
    {\
        bool stop = false;\
        LuaFocusEvent le(e);\
        focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnBlur);\
    }\
    void onFocus(T * sender, FocusEvent & e) override\
    {\
        bool stop = false;\
        LuaFocusEvent le(e);\
        focus_[le.phase()] ? focus_[le.phase()]->onFocusOut(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnFocus);\
    }\
    void onWheel(T * sender, WheelEvent & e) override\
    {\
        bool stop = false;\
        LuaWheelEvent le(e);\
        wheel_[le.phase()] ? wheel_[le.phase()]->onWheel(this, le, stop) : 0;\
        if (stop)\
            return;\
        PostEventToScript(this, le, kMethodOnWheel);\
    }\
    void onSizeChanged(T * sender, const Size & size) override\
    {\
        bool stop = false;\
        BonesSize bs = { size.width(), size.height() };\
        notify_ ? notify_->onSizeChanged(this, bs, stop) : 0;\
        if (stop)\
            return;\
        auto l = LuaContext::State();\
        LUA_STACK_AUTO_CHECK(l);\
        LuaContext::GetLOFromCO(l, this);\
        lua_getfield(l, -1, kNotifyOrder);\
        if (lua_istable(l, -1))\
        {\
            lua_getfield(l, -1, kMethodOnSizeChanged);\
            lua_pushnil(l);\
            lua_copy(l, -4, -1);\
            lua_pushnumber(l, bs.width);\
            lua_pushnumber(l, bs.height);\
            LuaContext::SafeLOPCall(l, 3, 0);\
        }\
        lua_pop(l, 2);\
    }\
    void onPositionChanged(T * sender, const Point & loc) override\
    {\
        bool stop = false;\
        BonesPoint bp = { loc.x(), loc.y() };\
        notify_ ? notify_->onPositionChanged(this, bp, stop) : 0;\
        if (stop)\
            return;\
        auto l = LuaContext::State();\
        LUA_STACK_AUTO_CHECK(l);\
        LuaContext::GetLOFromCO(l, this);\
        lua_getfield(l, -1, kNotifyOrder);\
        if (lua_istable(l, -1))\
        {\
            lua_getfield(l, -1, kMethodOnPositionChanged);\
            lua_pushnil(l);\
            lua_copy(l, -4, -1);\
            lua_pushnumber(l, bp.x);\
            lua_pushnumber(l, bp.y);\
            LuaContext::SafeLOPCall(l, 3, 0);\
        }\
        lua_pop(l, 2);\
    }\
    bool onHitTest(T * sender, const Point & loc) override\
    {\
        bool stop = false;\
        BonesPoint bp = { loc.x(), loc.y() };\
        bool ht = notify_ ? notify_->onHitTest(this, bp, stop) : true;\
        if (stop)\
            return ht;\
            \
        auto l = LuaContext::State();\
        LUA_STACK_AUTO_CHECK(l);\
        LuaContext::GetLOFromCO(l, this);\
        lua_getfield(l, -1, kNotifyOrder);\
        if (lua_istable(l, -1))\
        {\
            lua_getfield(l, -1, kMethodOnHitTest);\
            lua_pushnil(l);\
            lua_copy(l, -4, -1);\
            lua_pushnumber(l, bp.x);\
            lua_pushnumber(l, bp.y);\
            auto count = LuaContext::SafeLOPCall(l, 3, 1);\
            if (count > 0)\
                ht = !!lua_toboolean(l, -1);\
            lua_pop(l, count);\
        }\
        lua_pop(l, 2);\
        return ht;\
    }\
protected:\
    NotifyListener * notify_;\
    MouseListener * mouse_[BonesEvent::kPhaseCount];\
    KeyListener * key_[BonesEvent::kPhaseCount];\
    FocusListener * focus_[BonesEvent::kPhaseCount];\
    WheelListener * wheel_[BonesEvent::kPhaseCount];\


#define LUA_HANDLER_INIT()\
    notify_ = nullptr;\
    memset(mouse_, 0, sizeof(mouse_));\
    memset(key_, 0, sizeof(key_));\
    memset(focus_, 0, sizeof(focus_));\
    memset(wheel_, 0, sizeof(wheel_));\
    

}


#endif