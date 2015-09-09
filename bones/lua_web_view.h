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

    void setListener(NotifyListener * listener) override;
private:
    NotifyListener * listener_;
};
}

#endif