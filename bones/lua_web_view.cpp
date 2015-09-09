#include "lua_web_view.h"

namespace bones
{

static const char * kMetaTableWebView = "__mt_webview";

LuaWebView::LuaWebView(WebView * ob)
:LuaObject(ob, kMetaTableWebView), listener_(nullptr)
{

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

void LuaWebView::setListener(NotifyListener * listener)
{
    listener_ = listener;
}

}