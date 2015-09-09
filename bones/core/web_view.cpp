#include "web_view.h"
#include "SkCanvas.h"
#include "SkPaint.h"
#include "helper.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_version.h"
#include "include/capi/cef_app_capi.h"

namespace bones
{

class Browser : public CefClient,
                public CefLifeSpanHandler,
                public CefRenderHandler
{
public:
    Browser()
        :web_view_(nullptr)
    {
        ;
    }

    void setHost(WebView * wv)
    {
        web_view_ = wv;
    }

    void wasResized()
    {
        if (browser_)
            browser_->GetHost()->WasResized();
    }

    Pixmap & getPixmap()
    {
        return pixmap_;
    }

    bool Create()
    {
        CefWindowInfo info;
        info.SetAsWindowless(NULL, true);

        info.x = 0;
        info.y = 0;
        info.width = 0;
        info.height = 0;
        return !!(browser_ = CefBrowserHost::CreateBrowserSync(info, this,
            "http://www.baidu.com", CefBrowserSettings(), nullptr));
    }

    void Destroy()
    {
        if (browser_)
            browser_->GetHost()->CloseBrowser(true);
        browser_ = nullptr;
    }

    CefBrowser * cef()
    {
        return browser_;
    }
public:
    //CefLifeSpanHandler
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
    {
        return this;
    }

    CefRefPtr<CefRenderHandler> GetRenderHandler()
    {
        return this;
    }

    // CefLifeSpanHandler methods
    void OnAfterCreated(CefRefPtr<CefBrowser> browser)
    {
        browser_ = browser;
    }

    void OnBeforeClose(CefRefPtr<CefBrowser> browser)
    {//crash后 浏览器无反应了

    }

    // CefRenderHandler methods
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        if (web_view_)
        {
            rect.x = rect.y = 0;
            rect.width = static_cast<int>(web_view_->getWidth());
            rect.height = static_cast<int>(web_view_->getHeight());
            return true;
        }
        return true;
    }

    void OnPaint(CefRefPtr<CefBrowser> browser,
        PaintElementType type,
        const RectList & dirtyRects,
        const void* buffer, int width, int height) override
    {
        if (type != PET_VIEW)
            return;
        adjustPixmap(width, height);
        Pixmap::LockRec lr;
        if (pixmap_.lock(lr))
        {
            auto cef_pitch = width * 4;
            auto dst_pitch = (int)lr.pitch;
            auto pitch = dst_pitch > cef_pitch ? cef_pitch : dst_pitch;
            for (auto i = 0; i < height; ++i)
            {//按行拷贝
                const char * cbuffer = (const char *)buffer +  i * cef_pitch;
                memcpy((char *)lr.bits + i * lr.pitch, cbuffer, pitch);
            }
            pixmap_.unlock();
        }
        web_view_ ? web_view_->inval() : 0;
    }

    void OnCursorChange(CefRefPtr<CefBrowser> browser,
        CefCursorHandle cursor,
        CursorType type,
        const CefCursorInfo & custom_cursor_info) override
    {
        web_view_ ? web_view_->setCursor(cursor) : 0;
    }
private:
    void adjustPixmap(int width, int height)
    {
        if (pixmap_.getHeight() != width ||
            pixmap_.getWidth() != height)
        {
            pixmap_.free();
            if (width && height)
                pixmap_.allocate(width, height);
        }
    }
private:
    CefRefPtr<CefBrowser> browser_;
    WebView * web_view_;
    Pixmap pixmap_;

    IMPLEMENT_REFCOUNTING(Browser);
    friend class WebView;
};

bool WebView::StartUp()
{
    //不启用cef的话直接返回
    if (!Core::EnableCEF())
        return true;

    CefMainArgs args;
    CefSettings settings;
    settings.no_sandbox = true;
    settings.multi_threaded_message_loop = false;
    // Enable dev tools
    //settings.remote_debugging_port = 8088;
    // Use cefclient.exe to run render process and other sub processes.
    CefString(&settings.browser_subprocess_path) = "cefclient.exe";
    // Load locales/zh-CN.pak
    CefString(&settings.locale) = "zh-CN";

    return CefInitialize(args, settings, nullptr, nullptr);
}

void WebView::ShutDown()
{
    if (!Core::EnableCEF())
        return;
    CefShutdown();
}

void WebView::Update()
{
    if (!Core::EnableCEF())
        return;
    CefDoMessageLoopWork();
}

WebView::WebView()
{
    // Browser initialization.
    browser_ = new Browser;
    //browser 居然new的时候没有引用计数+1
    browser_->AddRef();
    assert(browser_->HasOneRef());
    if (browser_)
    {
        browser_->setHost(this);
        browser_->Create();
    }
        
        
}

WebView::~WebView()
{
    if (browser_)
    {
        browser_->Destroy();
        browser_->setHost(nullptr);        
        browser_->Release();
    }
}

const char * WebView::getClassName() const
{
    return kClassWebView;
}

void WebView::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    if (0 == opacity)
        return;
    auto & pm = browser_->getPixmap();
    if (!pm.isValid())
        return;
    SkPaint paint;
    paint.setAlpha(ClampAlpha(opacity));
    canvas.drawBitmap(Helper::ToSkBitmap(pm), 0, 0, &paint);
}

void WebView::onSizeChanged()
{
    browser_->wasResized();
}

void WebView::onMouseEnter(MouseEvent & e)
{

}

void WebView::onMouseLeave(MouseEvent & e)
{
    CefMouseEvent ce;
    ce.x = e.getLoc().x();
    ce.y = e.getLoc().y();
    ce.modifiers = 0;
    browser_->cef()->GetHost()->SendMouseMoveEvent(ce, true);
}

void WebView::onMouseMove(MouseEvent & e)
{
    CefMouseEvent ce;
    ce.x = e.getLoc().x();
    ce.y = e.getLoc().y();
    ce.modifiers = 0;
    browser_->cef()->GetHost()->SendMouseMoveEvent(ce, false);
}

void WebView::onMouseDown(MouseEvent & e)
{

}

void WebView::onMouseUp(MouseEvent & e)
{

}

void WebView::onFocus(FocusEvent & e)
{

}

void WebView::onBlur(FocusEvent & e)
{

}

void WebView::onKeyDown(KeyEvent & e)
{

}

void WebView::onKeyUp(KeyEvent & e)
{

}

void WebView::onKeyPress(KeyEvent & e)
{

}



}