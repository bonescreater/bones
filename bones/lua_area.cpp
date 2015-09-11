#include "lua_area.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_event.h"
#include "core/area.h"
#include "core/logging.h"

namespace bones
{

static const char * kMetaTableArea = "__mt_area";
static const char * kStrCapture = "capture";
static const char * kStrTarget = "target";
static const char * kStrBubbling = "bubbling";

static const char * ToEventPhaseStr(BonesEvent::Phase phase)
{
    if (BonesEvent::kCapture == phase)
        return kStrCapture;
    if (BonesEvent::kBubbling == phase)
        return kStrBubbling;
    if (BonesEvent::kTarget == phase)
        return kStrTarget;
    return "";
}

LuaArea::LuaArea(Area * ob)
:LuaObject(ob), mouse_(nullptr),
key_(nullptr), focus_(nullptr), wheel_(nullptr),
notify_(nullptr)
{
    createLuaTable();
}

void LuaArea::addEvent(const char * name,
    const char * phase,
    const char * mod,
    const char * func)
{
    if (!name || !phase || !func)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    assert(lua_istable(l, -1));
    lua_getfield(l, -1, kEventOrder);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        lua_newtable(l);
        assert(lua_istable(l, -2));
        lua_setfield(l, -2, kEventOrder);
        lua_getfield(l, -1, kEventOrder);
    }
    lua_getfield(l, -1, phase);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        lua_newtable(l);
        assert(lua_istable(l, -2));
        lua_setfield(l, -2, phase);
        lua_getfield(l, -1, phase);
    }

    lua_pushnil(l);
    if (mod)
    {
        lua_getglobal(l, mod);
        assert(lua_istable(l, -1));
        lua_getfield(l, -1, func);
        lua_copy(l, -1, -3);
        lua_pop(l, 2);
    }
    else
    {
        lua_getglobal(l, func);
        lua_copy(l, -1, -2);
        lua_pop(l, 1);
    }
    assert(lua_isfunction(l, -1));
    lua_setfield(l, -2, name);

    lua_pop(l, 3);
}

LuaArea::~LuaArea()
{
    object_->setDelegate(nullptr);
}

BonesArea::NotifyListener * LuaArea::getNotify() const
{
    return notify_;
}

void LuaArea::notifyCreate()
{
    object_->setDelegate(this);
    LuaObject::notifyCreate();
}

void LuaArea::notifyDestroy()
{
    LuaObject::notifyDestroy();
    object_->setDelegate(nullptr);
}

void LuaArea::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableArea);
}

void LuaArea::setListener(MouseListener * lis)
{
    mouse_ = lis;
}

void LuaArea::setListener(KeyListener * lis)
{
    key_ = lis;
}

void LuaArea::setListener(FocusListener * lis)
{
    focus_ = lis;
}

void LuaArea::setListener(WheelListener * lis)
{
    wheel_ = lis;
}

void LuaArea::setListener(NotifyListener * notify)
{
    notify_ = notify;
}

static int event_stack_count = 0;

class EventStack
{
public:
    EventStack(){event_stack_count++; }

    ~EventStack(){ assert(event_stack_count); event_stack_count--; }

    int getCount() const{ return event_stack_count; }
};

template<class T>
static void PostToScript(LuaArea * ob, T & e, const char * type)
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
            T::GetMetaTable(l);
            lua_setmetatable(l, -2);
            LuaContext::SafeLOPCall(l, 2, 0);
        }
        lua_pop(l, 1);
    }
    lua_pop(l, 2);
}

void LuaArea::onMouseEnter(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseEnter(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseEnter);
}

void LuaArea::onMouseMove(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseMove(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseMove);
}

void LuaArea::onMouseDown(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseDown(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseDown);
}

void LuaArea::onMouseUp(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseUp(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseUp);
}

void LuaArea::onMouseClick(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseClick(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseClick);
}

void LuaArea::onMouseDClick(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseDClick(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseDClick);
}

void LuaArea::onMouseLeave(Area * sender, MouseEvent & e)
{
    bool stop = false;
    LuaMouseEvent le(e);
    mouse_ ? mouse_->onMouseLeave(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnMouseLeave);
}

void LuaArea::onKeyDown(Area * sender, KeyEvent & e)
{
    bool stop = false;
    LuaKeyEvent le(e);
    key_ ? key_->onKeyDown(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnKeyDown);
}

void LuaArea::onKeyUp(Area * sender, KeyEvent & e)
{
    bool stop = false;
    LuaKeyEvent le(e);
    key_ ? key_->onKeyUp(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnKeyUp);
}

void LuaArea::onChar(Area * sender, KeyEvent & e)
{
    bool stop = false;
    LuaKeyEvent le(e);
    key_ ? key_->onKeyPress(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnChar);
}

void LuaArea::onFocusOut(Area * sender, FocusEvent & e)
{
    bool stop = false;
    LuaFocusEvent le(e);
    focus_ ? focus_->onFocusOut(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnFocusOut);
}

void LuaArea::onFocusIn(Area * sender, FocusEvent & e)
{
    bool stop = false;
    LuaFocusEvent le(e);
    focus_ ? focus_->onFocusOut(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnFocusIn);
}

void LuaArea::onBlur(Area * sender, FocusEvent & e)
{
    bool stop = false;
    LuaFocusEvent le(e);
    focus_ ? focus_->onFocusOut(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnBlur);
}

void LuaArea::onFocus(Area * sender, FocusEvent & e)
{
    bool stop = false;
    LuaFocusEvent le(e);
    focus_ ? focus_->onFocusOut(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnFocus);
}

void LuaArea::onWheel(Area * sender, WheelEvent & e)
{
    bool stop = false;
    LuaWheelEvent le(e);
    wheel_ ? wheel_->onWheel(this, le, stop) : 0;
    if (stop)
        return;
    PostToScript(this, le, kMethodOnWheel);
}

void LuaArea::onSizeChanged(Area * sender, const Size & size)
{
    bool stop = false;
    BonesSize bs = { size.width(), size.height() };
    notify_ ? notify_->onSizeChanged(this, bs, stop) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodOnSizeChanged);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, bs.width);
        lua_pushnumber(l, bs.height);
        LuaContext::SafeLOPCall(l, 3, 0);
    }
    lua_pop(l, 2);
}

void LuaArea::onPositionChanged(Area * sender, const Point & loc)
{
    bool stop = false;
    BonesPoint bp = { loc.x(), loc.y() };
    notify_ ? notify_->onPositionChanged(this, bp, stop) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodOnPositionChanged);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
        LuaContext::SafeLOPCall(l, 3, 0);
    }
    lua_pop(l, 2);
}

bool LuaArea::onHitTest(Area * sender, const Point & loc)
{
    bool stop = false;
    BonesPoint bp = { loc.x(), loc.y() };
    bool ht = notify_ ? notify_->onHitTest(this, bp, stop) : true;
    if (stop)
        return ht;

    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodOnHitTest);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
        auto count = LuaContext::SafeLOPCall(l, 3, 1);
        if (count > 0)
            ht = !!lua_toboolean(l, -1);
        lua_pop(l, count);
    }
    lua_pop(l, 2);
    return ht;
}


}