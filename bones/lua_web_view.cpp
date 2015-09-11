#include "lua_web_view.h"
#include "core/encoding.h"

namespace bones
{

static const char * kMetaTableWebView = "__mt_webview";

static int Open(lua_State * l)
{
    lua_settop(l, 1);
    lua_pushnil(l);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaWebView * bob = static_cast<LuaWebView *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        lua_pushboolean(l, bob->open());
    return 1;
}

static int Close(lua_State * l)
{
    lua_settop(l, 1);

    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaWebView * bob = static_cast<LuaWebView *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        bob->close();
    return 0;
}

static int LoadURL(lua_State * l)
{
    lua_settop(l, 2);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaWebView * bob = static_cast<LuaWebView *>(
        LuaContext::CallGetCObject(l));
    if (bob)
        bob->loadURL(Encoding::FromUTF8(lua_tostring(l, 2)).data());
    return 0;
}

static int ExecuteJS(lua_State * l)
{
    lua_settop(l, 4);
    lua_pushnil(l);
    lua_copy(l, 1, -1);
    LuaWebView * bob = static_cast<LuaWebView *>(
        LuaContext::CallGetCObject(l));
    if (bob)
    {
        bob->executeJS(Encoding::FromUTF8(lua_tostring(l, 2)).data(),
                       Encoding::FromUTF8(lua_tostring(l, 3)).data(),
                       static_cast<int>(lua_tointeger(l, 4)));
    }
        
    return 0;
}

static const char * kMethodOpen = "open";
static const char * kMethodClose = "close";
static const char * kMethodLoadURL = "loadURL";

LuaWebView::LuaWebView(WebView * ob)
:LuaObject(ob), listener_(nullptr)
{
    createLuaTable();
}

LuaWebView::~LuaWebView()
{

}

LuaWebView::NotifyListener * LuaWebView::getNotify() const
{
    return listener_;
}

void LuaWebView::notifyCreate()
{
    //object_->setDelegate(this);
    LuaObject::notifyCreate();
}

void LuaWebView::notifyDestroy()
{
    LuaObject::notifyDestroy();
    //object_->setDelegate(nullptr);
}

void LuaWebView::createMetaTable(lua_State * l)
{
    if(!LuaObject::createMetaTable(l, kMetaTableWebView))
    {
        lua_pushcfunction(l, &Open);
        lua_setfield(l, -2, kMethodOpen);
        lua_pushcfunction(l, &Close);
        lua_setfield(l, -2, kMethodClose);
        lua_pushcfunction(l, &LoadURL);
        lua_setfield(l, -2, kMethodLoadURL);
    }
}

void LuaWebView::setListener(NotifyListener * listener)
{
    listener_ = listener;
}

bool LuaWebView::open()
{
    return object_->open();
}

void LuaWebView::close()
{
    object_->close();
}

void LuaWebView::loadURL(const wchar_t * url)
{
    object_->loadURL(url);
}

void LuaWebView::executeJS(const wchar_t * code,
                           const wchar_t * url,
                           int start_line)
{
    object_->executeJS(code, url, start_line);
}

}