#ifndef BONES_LUA_WEB_VIEW_H_
#define BONES_LUA_WEB_VIEW_H_

#include "lua_handler.h"
#include "core/web_view.h"

namespace bones
{

class LuaWebView : public LuaObject<BonesWebView, WebView>,
                   public WebView::Delegate
{
public:
    LuaWebView(WebView * ob);

    void createMetaTable(lua_State * l) override;

    bool open() override;

    void close() override;

    void loadURL(const wchar_t * url) override;

    void executeJS(const wchar_t * code,
                   const wchar_t * url,
                   int start_line) override;

    LUA_HANDLER(WebView);
};
}

#endif