#include "lua_root.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/root.h"
#include "core/helper.h"
#include "core/event.h"

#include "script_parser.h"
#include "utils.h"

namespace bones
{

static const char * kMetaTableRoot = "__mt_root";

static const char * kMethodRequestFocus = "requestFocus";
static const char * kMethodShiftFocus = "shiftFocus";
static const char * kMethodInvalidRect = "invalidRect";
static const char * kMethodChangeCursor = "changeCursor";
static const char * kMethodChangeCaretPos = "changeCaretPos";

//(self, color)
static int SetColor(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaRoot * bob = static_cast<LuaRoot *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        bob->setColor(static_cast<BonesColor>(lua_tointeger(l, 2)));
    return 0;
}

LuaRoot::LuaRoot(Root * ob)
:LuaObject(ob)
{
    LUA_HANDLER_INIT();
    createLuaTable();
}

LuaRoot::~LuaRoot()
{
    ;
}

void LuaRoot::createMetaTable(lua_State * l)
{
    if (!LuaObject::createMetaTable(l, kMetaTableRoot))
    {
        lua_pushcfunction(l, SetColor);
        lua_setfield(l, -2, kMethodSetColor);
    }
}

void LuaRoot::setColor(BonesColor color)
{
    object_->setColor(color);
}

void LuaRoot::attachTo(BonesWidget hwnd)
{
    object_->attachTo(hwnd);
}

bool LuaRoot::isDirty() const
{
    return object_->isDirty();
}

BonesRect LuaRoot::getDirtyRect() const
{   
    auto & ob = object_->getDirtyRect();
    BonesRect br = { ob.left(), ob.top(), ob.right(), ob.bottom() };
    return br;
}
void LuaRoot::draw()
{
    object_->draw();
}

HBITMAP LuaRoot::getBackBuffer() const
{
    return Helper::ToHBITMAP(object_->getBackBuffer());
}

void LuaRoot::getBackBuffer(const void * & data, size_t & pitch) const
{
    object_->getBackBuffer(data, pitch);
}

void LuaRoot::sendMouse(MouseMessage msg, const BonesPoint & pt, int flags)
{
    EventType type = kET_COUNT;
    MouseButton mb = kMB_NONE;
    Point p = { pt.x, pt.y };
    Utils::ToEMForMouse(msg, type, mb);
    MouseEvent e(type, mb, object_.get(), p, p, flags);
    object_->sendMouse(e);
}

void LuaRoot::sendKey(KeyMessage msg, int32_t vk, uint32_t states, int flags)
{
    EventType type = kET_COUNT;
    if (kKeyDown == msg || kSysKeyDown == msg)
        type = kET_KEY_DOWN;
    else if (kKeyUp == msg || kSysKeyUp == msg)
        type = kET_KEY_UP;
    else if (kChar == msg || kSysChar == msg)
        type = kET_CHAR;
    else
        return;
    bool system = (BonesRoot::kSysChar == msg) || (BonesRoot::kSysKeyDown == msg) || 
        (BonesRoot::kSysKeyUp == msg);
    KeyEvent ke(type, object_.get(), (KeyboardCode)vk, *(KeyState *)(&states), 
        system, flags);
    object_->sendKey(ke);
}

void LuaRoot::sendWheel(int dx, int dy, const BonesPoint & pt, int flags)
{
    Point p = { pt.x, pt.y };
    WheelEvent we(kET_MOUSE_WHEEL, object_.get(), dx, dy, p, p, flags);
    object_->sendWheel(we);
}

void LuaRoot::sendFocus(bool focus)
{
    object_->sendFocus(focus);
}

bool LuaRoot::sendComposition(IMEMessage msg, const IMEInfo * info)
{
    //NativeEvent e = { msg, wparam, lparam, 0 };
    EventType type = kET_COUNT;
    if (BonesRoot::kCompositionStart == msg)
        type = kET_COMPOSITION_START;
    else if (BonesRoot::kCompositionUpdate == msg)
        type = kET_COMPOSITION_UPDATE;
    else if (BonesRoot::kCompositionEnd == msg)
        type = kET_COMPOSITION_END;
    else
        return false;

    CompositionEvent e(type, object_.get(),
        info ? info->index : 0,
        info ? info->dbcs : 0,
        info ? info->str : nullptr,
        info ? info->cursor : 0);
    return object_->sendComposition(e);
}

void LuaRoot::requestFocus(Root * sender)
{
    bool stop = false;
    notify_ ? notify_->requestFocus(this, stop) : 0;
    if (stop)
        return;
    //post to script
    //(self)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodRequestFocus);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        LuaContext::SafeLOPCall(l, 1, 0);
    }

    lua_pop(l, 2);
}

void LuaRoot::shiftFocus(Root * sender, View * prev, View * current)
{
    bool stop = false;
    BonesObject * bo_prev = GetCoreInstance()->getObject(prev);
    BonesObject * bo_current = GetCoreInstance()->getObject(current);

    notify_ ? notify_->shiftFocus(this, bo_prev, bo_current, stop) : 0;
    if (stop)
        return;

    //post to script
    //(self)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodShiftFocus);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        LuaContext::GetLOFromCO(l, bo_prev);
        LuaContext::GetLOFromCO(l, bo_current);
        LuaContext::SafeLOPCall(l, 3, 0);
    }

    lua_pop(l, 2);
}

void LuaRoot::changeCaretPos(Root * sender, const Point & pt)
{
    bool stop = false;
    BonesPoint bp = { pt.x(), pt.y() };
    notify_ ? notify_->changeCaretPos(this, bp, stop) : 0;
    if (stop)
        return;

    //post to script
    //(self)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodChangeCaretPos);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, bp.x);
        lua_pushnumber(l, bp.y);
        LuaContext::SafeLOPCall(l, 3, 0);
    }

    lua_pop(l, 2);
}

void LuaRoot::invalidRect(Root * sender, const Rect & rect)
{
    bool stop = false;
    BonesRect r = { rect.left(), rect.top(), rect.right(), rect.bottom() };
    notify_ ? notify_->invalidRect(this, r, stop) : 0;
    if (stop)
        return;
    //(self, l, t, r, b)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodInvalidRect);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, r.left);
        lua_pushnumber(l, r.top);
        lua_pushnumber(l, r.right);
        lua_pushnumber(l, r.bottom);
        LuaContext::SafeLOPCall(l, 5, 0);
    }
    lua_pop(l, 2);
}

void LuaRoot::changeCursor(Root * sender, View::Cursor cursor, void * content)
{
    bool stop = false;
    notify_ ? notify_->changeCursor(this, 
        static_cast<BonesCursor>(cursor), content, stop) : 0;
    if (stop)
        return;
    //(self, cursor)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodChangeCursor);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushinteger(l, (lua_Integer)cursor);
        lua_pushlightuserdata(l, content);
        LuaContext::SafeLOPCall(l, 3, 0);
    }
    lua_pop(l, 2);
}

//void LuaRoot::createCaret(Root * sender, Caret caret, const Size & size)
//{
//    bool stop = false;
//    BonesSize bs = { size.width(), size.height() };
//    listener_ ? listener_->createCaret(this, caret, bs, stop) : 0;
//    if (stop)
//        return;
//    //(self, cursor)
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodcreateCaret);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        lua_pushinteger(l, (lua_Integer)caret);
//        lua_pushnumber(l, size.width());
//        lua_pushnumber(l, size.height());
//        LuaContext::SafeLOPCall(l, 4, 0);
//    }
//    lua_pop(l, 2);
//}
//
//void LuaRoot::showCaret(Root * sender, bool show)
//{
//    bool stop = false;
//    listener_ ? listener_->showCaret(this, show, stop) : true;
//    if (stop)
//        return;
//    //(self, cursor)
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodshowCaret);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        lua_pushboolean(l, show);
//        LuaContext::SafeLOPCall(l, 2, 0);
//    }
//    lua_pop(l, 2);
//}
//
//void LuaRoot::changeCaretPos(Root * sender, const Point & pt)
//{
//    bool stop = false;
//    BonesPoint bp = { pt.x(), pt.y() };
//    listener_ ? listener_->changeCaretPos(this, bp, stop) : 0;
//    if (stop)
//        return;
//    //(self, cursor)
//    auto l = LuaContext::State();
//    LUA_STACK_AUTO_CHECK(l);
//    LuaContext::GetLOFromCO(l, this);
//    lua_getfield(l, -1, kNotifyOrder);
//    if (lua_istable(l, -1))
//    {
//        lua_getfield(l, -1, kMethodchangeCaretPos);
//        lua_pushnil(l);
//        lua_copy(l, -4, -1);
//        lua_pushnumber(l, pt.x());
//        lua_pushnumber(l, pt.y());
//        LuaContext::SafeLOPCall(l, 3, 0);
//    }
//    lua_pop(l, 2);
//}

//void LuaRoot::onSizeChanged(Root * sender, const Size & size)
//{
//    bool stop = false;
//    BonesSize bs = { size.width(), size.height() };
//    listener_ ? listener_->onSizeChanged(this, bs, stop) : 0;
//    if (stop)
//        return;
//    //(self, w, h)
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
//void LuaRoot::onPositionChanged(Root * sender, const Point & loc)
//{
//    bool stop = false;
//    BonesPoint bp = { loc.x(), loc.y() };
//    listener_ ? listener_->onPositionChanged(this, bp, stop) : 0;
//    if (stop)
//        return;
//    //(self, w, h)
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


}