#include "lua_root.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/root.h"
#include "core/helper.h"
#include "script_parser.h"
namespace bones
{

static const char * kMetaTableRoot = "__mt_root";

static const char * kMethodRequestFocus = "requestFocus";
static const char * kMethodInvalidRect = "invalidRect";
static const char * kMethodChangeCursor = "changeCursor";
static const char * kMethodcreateCaret = "createCaret";
static const char * kMethodshowCaret = "showCaret";
static const char * kMethodchangeCaretPos = "changeCaretPos";


LuaRoot::LuaRoot(Root * ob)
:LuaObject(ob), listener_(nullptr)
{
    createLuaTable();
}

LuaRoot::~LuaRoot()
{
    ;
}

BonesRoot::NotifyListener * LuaRoot::getNotify() const
{
    return listener_;
}

void LuaRoot::notifyCreate() 
{
    object_->setDelegate(this);
    LuaObject::notifyCreate();
}

void LuaRoot::notifyDestroy() 
{
    LuaObject::notifyDestroy();
    object_->setDelegate(nullptr);
}

void LuaRoot::createMetaTable(lua_State * l)
{
    LuaObject::createMetaTable(l, kMetaTableRoot);
}

void LuaRoot::setListener(NotifyListener * listener)
{
    listener_ = listener;
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

bool LuaRoot::handleMouse(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    return object_->handleMouse(e);
}

bool LuaRoot::handleKey(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    return object_->handleKey(e);
}

bool LuaRoot::handleFocus(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    return object_->handleFocus(e);
}

bool LuaRoot::handleComposition(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    return object_->handleComposition(e);
}

bool LuaRoot::handleWheel(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    return object_->handleWheel(e);
}

void LuaRoot::requestFocus(Root * sender)
{
    bool stop = false;
    listener_ ? listener_->requestFocus(this, stop): 0;
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

void LuaRoot::invalidRect(Root * sender, const Rect & rect)
{
    bool stop = false;
    BonesRect r = { rect.left(), rect.top(), rect.right(), rect.bottom() };
    listener_ ? listener_->invalidRect(this, r, stop) : 0;
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

void LuaRoot::changeCursor(Root * sender, Cursor cursor)
{
    bool stop = false;
    listener_ ? listener_->changeCursor(this, cursor, stop) : 0;
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
        LuaContext::SafeLOPCall(l, 2, 0);
    }
    lua_pop(l, 2);
}

void LuaRoot::createCaret(Root * sender, Caret caret, const Size & size)
{
    bool stop = false;
    BonesSize bs = { size.width(), size.height() };
    listener_ ? listener_->createCaret(this, caret, bs, stop) : 0;
    if (stop)
        return;
    //(self, cursor)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodcreateCaret);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushinteger(l, (lua_Integer)caret);
        lua_pushnumber(l, size.width());
        lua_pushnumber(l, size.height());
        LuaContext::SafeLOPCall(l, 4, 0);
    }
    lua_pop(l, 2);
}

void LuaRoot::showCaret(Root * sender, bool show)
{
    bool stop = false;
    listener_ ? listener_->showCaret(this, show, stop) : true;
    if (stop)
        return;
    //(self, cursor)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodshowCaret);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushboolean(l, show);
        LuaContext::SafeLOPCall(l, 2, 0);
    }
    lua_pop(l, 2);
}

void LuaRoot::changeCaretPos(Root * sender, const Point & pt)
{
    bool stop = false;
    BonesPoint bp = { pt.x(), pt.y() };
    listener_ ? listener_->changeCaretPos(this, bp, stop) : 0;
    if (stop)
        return;
    //(self, cursor)
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodchangeCaretPos);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushnumber(l, pt.x());
        lua_pushnumber(l, pt.y());
        LuaContext::SafeLOPCall(l, 3, 0);
    }
    lua_pop(l, 2);
}

void LuaRoot::onSizeChanged(Root * sender, const Size & size)
{
    bool stop = false;
    BonesSize bs = { size.width(), size.height() };
    listener_ ? listener_->onSizeChanged(this, bs, stop) : 0;
    if (stop)
        return;
    //(self, w, h)
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

void LuaRoot::onPositionChanged(Root * sender, const Point & loc)
{
    bool stop = false;
    BonesPoint bp = { loc.x(), loc.y() };
    listener_ ? listener_->onPositionChanged(this, bp, stop) : 0;
    if (stop)
        return;
    //(self, w, h)
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


}