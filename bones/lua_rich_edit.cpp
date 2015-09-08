#include "lua_rich_edit.h"
#include "lua_context.h"
#include "lua_check.h"
#include "core/rich_edit.h"

namespace bones
{

static const char * kMetaTableRichEdit = "__mt_richedit";
static const char * kMethodshowCaret = "showCaret";
static const char * kMethodcreateCaret = "createCaret";
static const char * kMethodsetCaretPos = "setCaretPos";
static const char * kMethodscreenToClient = "screenToClient";
static const char * kMethodclientToScreen = "clientToScreen";
static const char * kMethodimmGetContext = "immGetContext";
static const char * kMethodimmReleaseContext = "immReleaseContext";
static const char * kMethodtxNotify = "txNotify";
static const char * kMethodonReturn = "onReturn";

LuaRichEdit::LuaRichEdit(RichEdit * ob)
:LuaObject(ob, kMetaTableRichEdit), listener_(nullptr)
{
    ;
}

LuaRichEdit::~LuaRichEdit()
{
    ;
}

BonesRichEdit::NotifyListener * LuaRichEdit::getNotify() const
{
    return listener_;
}

void LuaRichEdit::notifyCreate()
{
    object_->setDelegate(this);
    LuaObject::notifyCreate();
}

void LuaRichEdit::notifyDestroy()
{
    LuaObject::notifyDestroy();
    object_->setDelegate(nullptr);
}

void LuaRichEdit::setListener(NotifyListener * listener)
{
    listener_ = listener;
}

BOOL LuaRichEdit::screenToClient(RichEdit * sender, LPPOINT lppt)
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
    lua_pop(l, 2);
    return bret;
}

BOOL LuaRichEdit::clientToScreen(RichEdit * sender, LPPOINT lppt)
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
    lua_pop(l, 2);
    return bret;
}

HIMC LuaRichEdit::immGetContext(RichEdit * sender)
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
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        auto count = LuaContext::SafeLOPCall(l, 1, 1);
        if (count > 0)
            imc = (HIMC)(lua_tointeger(l, -1));
    }
    lua_pop(l, 2);

    return imc;
}

void LuaRichEdit::immReleaseContext(RichEdit * sender, HIMC himc)
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

HRESULT LuaRichEdit::txNotify(RichEdit * sender, DWORD iNotify, void  *pv)
{
    bool stop = false;
    HRESULT result = listener_ ? listener_->txNotify(this, iNotify, pv, stop) : 0;
    if (stop)
        return result;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodtxNotify);
        lua_pushnil(l);
        lua_copy(l, -4, -1);
        lua_pushinteger(l, iNotify);
        lua_pushinteger(l, (lua_Integer)pv);

        auto count = LuaContext::SafeLOPCall(l, 3, 1);
        if (count > 0)
            result = (HRESULT)lua_tointeger(l, -1);
        lua_pop(l, count);
    }
    lua_pop(l, 2);

    return result;
}

void LuaRichEdit::onReturn(RichEdit * sender)
{
    bool stop = false;
    listener_ ? listener_->onReturn(this, stop) : 0;
    if (stop)
        return;
    auto l = LuaContext::State();
    LUA_STACK_AUTO_CHECK(l);
    LuaContext::GetLOFromCO(l, this);
    lua_getfield(l, -1, kNotifyOrder);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, kMethodonReturn);
        lua_pushnil(l);
        lua_copy(l, -4, -1);

        LuaContext::SafeLOPCall(l, 1, 0);
    }
    lua_pop(l, 2);
}

}