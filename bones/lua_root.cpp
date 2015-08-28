#include "lua_root.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/root.h"
#include "core/helper.h"
#include "lua_meta_table.h"
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

static const char * kMethodOnSizeChanged = "onSizeChanged";
static const char * kMethodOnPositionChanged = "onPositionChanged";


LuaRoot::LuaRoot(Root * ob)
:LuaObject(ob, kMetaTableRoot), listener_(nullptr)
{
    ob->setDelegate(this);
}

LuaRoot::~LuaRoot()
{
    object_->setDelegate(nullptr);
}

void LuaRoot::addNotify(const char * notify_name, const char * mod, const char * func)
{
    if (!notify_name || !func)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    assert(lua_istable(l, -1));

    lua_getfield(l, -1, kNotifyOrder);
    if (!lua_istable(l, -1))
    {
        lua_pop(l, 1);
        lua_newtable(l);
        assert(lua_istable(l, -2));
        lua_setfield(l, -2, kNotifyOrder);
        lua_getfield(l, -1, kNotifyOrder);
    }
    lua_pushnil(l);
    //得到func
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
    lua_setfield(l, -2, notify_name);
    lua_pop(l, 2);
}

void LuaRoot::setListener(Listener * listener)
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

HDC LuaRoot::dc() const
{
    object_->getBackBuffer();
    return Helper::ToHDC(object_->getBackBuffer());
}

void LuaRoot::handleMouse(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    object_->handleMouse(e);
}

void LuaRoot::handleKey(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    object_->handleKey(e);
}

void LuaRoot::handleFocus(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    object_->handleFocus(e);
}

void LuaRoot::handleComposition(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    object_->handleComposition(e);
}

void LuaRoot::handleWheel(UINT msg, WPARAM wparam, LPARAM lparam)
{
    NativeEvent e = { msg, wparam, lparam, 0 };
    object_->handleWheel(e);
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
    listener_ ? listener_->showCaret(this, show, stop) : 0;
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