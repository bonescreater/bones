#include "lua_rich_edit.h"
#include "lua_context.h"
#include "lua_check.h"
#include "lua_meta_table.h"
#include "core/rich_edit.h"

namespace bones
{

static const char * kMetaTableRichEdit = "__mt_richedit";
static const char * kMethodkillTimer = "killTimer";
static const char * kMethodsetTimer = "setTimer";
static const char * kMethodshowCaret = "showCaret";
static const char * kMethodcreateCaret = "createCaret";
static const char * kMethodsetCaretPos = "setCaretPos";
static const char * kMethodscreenToClient = "screenToClient";
static const char * kMethodclientToScreen = "clientToScreen";
static const char * kMethodimmGetContext = "immGetContext";
static const char * kMethodimmReleaseContext = "immReleaseContext";

LuaRichEdit::LuaRichEdit(RichEdit * ob)
:LuaObject(ob), listener_(nullptr)
{
    ob->setDelegate(this);
    LuaMetaTable::CreatLuaTable(LuaContext::State(), kMetaTableRichEdit, this);
}

LuaRichEdit::~LuaRichEdit()
{
    object_->setDelegate(nullptr);
}

void LuaRichEdit::setListener(Listener * listener)
{
    listener_ = listener;
}

void LuaRichEdit::killTimer(Ref * sender, UINT idTimer)
{
    bool stop = false;
    listener_ ? listener_->killTimer(this, idTimer, stop) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodkillTimer);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            lua_pushinteger(l, idTimer);
            LuaContext::SafeLOPCall(l, 2, 0);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
}

BOOL LuaRichEdit::setTimer(Ref * sender, UINT idTimer, UINT uTimeout)
{
    bool stop = false;
    BOOL bret = listener_ ? listener_->setTimer(this, idTimer, uTimeout, stop) : 0;
    if (stop)
        return bret;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodsetTimer);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            lua_pushinteger(l, idTimer);
            lua_pushinteger(l, uTimeout);
            auto count = LuaContext::SafeLOPCall(l, 3, 1);
            if (count > 0)
                bret = lua_toboolean(l, -1);
            lua_pop(l, count);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
    return bret;
}

BOOL LuaRichEdit::showCaret(Ref * sender, BOOL fshow)
{
    bool stop = false;
    BOOL bret = listener_ ? listener_->showCaret(this, fshow, stop) : 0;
    if (stop)
        return bret;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodshowCaret);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            lua_pushboolean(l, fshow);
            auto count = LuaContext::SafeLOPCall(l, 2, 1);
            if (count > 0)
                bret = lua_toboolean(l, -1);
            lua_pop(l, count);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
    return bret;
}

BOOL LuaRichEdit::createCaret(Ref * sender, HBITMAP hbmp, INT xWidth, INT yHeight)
{
    bool stop = false;
    BOOL bret = listener_ ? listener_->createCaret(this, hbmp, xWidth, yHeight, stop) : 0;
    if (stop)
        return bret;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodcreateCaret);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            lua_pushinteger(l, (lua_Integer)hbmp);
            lua_pushinteger(l, xWidth);
            lua_pushinteger(l, yHeight);

            auto count = LuaContext::SafeLOPCall(l, 4, 1);
            if (count > 0)
                bret = lua_toboolean(l, -1);
            lua_pop(l, count);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
    return bret;
}

BOOL LuaRichEdit::setCaretPos(Ref * sender, INT x, INT y)
{
    bool stop = false;
    BOOL bret = listener_ ? listener_->setCaretPos(this, x, y, stop) : 0;
    if (stop)
        return bret;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodsetCaretPos);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            lua_pushinteger(l, x);
            lua_pushinteger(l, y);

            auto count = LuaContext::SafeLOPCall(l, 3, 1);
            if (count > 0)
                bret = lua_toboolean(l, -1);
            lua_pop(l, count);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
    return bret;
}

BOOL LuaRichEdit::screenToClient(Ref * sender, LPPOINT lppt)
{
    bool stop = false;
    BOOL bret = listener_ ? listener_->screenToClient(this, lppt, stop) : 0;
    if (stop)
        return bret;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodscreenToClient);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            if (lppt)
            {
                lua_pushinteger(l, lppt->x);
                lua_pushinteger(l, lppt->y);
            }
            else
            {
                lua_pushnil(l);
                lua_pushnil(l);
            }
            auto count = LuaContext::SafeLOPCall(l, 3, LUA_MULTRET);
            if (count == 2)
            {
                bret = TRUE;
                if (lppt)
                {
                    lppt->x = (LONG) lua_tointeger(l, -2);
                    lppt->y = (LONG) lua_tointeger(l, -1);
                }
            }
            else
                bret = FALSE;
                
            lua_pop(l, count);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
    return bret;
}

BOOL LuaRichEdit::clientToScreen(Ref * sender, LPPOINT lppt)
{
    bool stop = false;
    BOOL bret = listener_ ? listener_->clientToScreen(this, lppt, stop) : 0;
    if (stop)
        return bret;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodclientToScreen);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            if (lppt)
            {
                lua_pushinteger(l, lppt->x);
                lua_pushinteger(l, lppt->y);
            }
            else
            {
                lua_pushnil(l);
                lua_pushnil(l);
            }
            auto count = LuaContext::SafeLOPCall(l, 3, LUA_MULTRET);
            if (count == 2)
            {
                bret = TRUE;
                if (lppt)
                {
                    lppt->x = (LONG)lua_tointeger(l, -2);
                    lppt->y = (LONG)lua_tointeger(l, -1);
                }
            }
            else
                bret = FALSE;

            lua_pop(l, count);
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);
    return bret;
}

HIMC LuaRichEdit::immGetContext(Ref * sender)
{
    bool stop = false;
    HIMC imc = listener_ ? listener_->immGetContext(this, stop) : 0;
    if (stop)
        return imc;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodimmGetContext);
        if (lua_isfunction(l, -1))
        {
            lua_pushnil(l);
            lua_copy(l, -4, -1);
            auto count = LuaContext::SafeLOPCall(l, 2, 1);
            if (count > 0)
                imc = (HIMC)(lua_tointeger(l, -1));
        }
        else
            lua_pop(l, 1);
    }
    lua_pop(l, 2);

    return imc;
}

void LuaRichEdit::immReleaseContext(Ref * sender, HIMC himc)
{
    bool stop = false;
    listener_ ? listener_->immReleaseContext(this, himc, stop) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodimmReleaseContext);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushinteger(l, (lua_Integer)himc);

        LuaContext::SafeLOPCall(l, 2, 0);
    }
    lua_pop(l, 2);
}

}