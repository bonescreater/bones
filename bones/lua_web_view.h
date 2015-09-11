#ifndef BONES_LUA_WEB_VIEW_H_
#define BONES_LUA_WEB_VIEW_H_

#include "lua_object.h"
#include "core/web_view.h"

namespace bones
{

class LuaWebView : public LuaObject<BonesWebView, BonesWebView::NotifyListener, WebView>
{
public:
    LuaWebView(WebView * ob);

    ~LuaWebView();

    NotifyListener * getNotify() const override;

    void notifyCreate() override;

    void notifyDestroy() override;

    void createMetaTable(lua_State * l) override;

    void setListener(NotifyListener * listener) override;

    bool open() override;

    void close() override;

    void loadURL(const wchar_t * url) override;
private:
    NotifyListener * listener_;
};
}

#endif