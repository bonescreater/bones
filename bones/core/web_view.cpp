#include "web_view.h"
#include "SkCanvas.h"
#include "SkPaint.h"
#include "helper.h"
#include "encoding.h"

#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/cef_version.h"

namespace bones
{

class Browser : public CefClient,
                public CefLifeSpanHandler,
                public CefRenderHandler,
                public CefFocusHandler
{
#define CHECK_CEF_BROWSER_HOST \
    if (!browser_)\
        return; \
    auto host = browser_->GetHost(); \
    if (!host)\
        return \

public:
    Browser(WebView * wv)
        :web_view_(wv)
    {
        assert(web_view_);
    }

    bool open()
    {
        CefWindowInfo info;
        info.SetAsWindowless(NULL, true);

        info.x = 0;
        info.y = 0;
        info.width = static_cast<int>(web_view_->getWidth());
        info.height = static_cast<int>(web_view_->getHeight());
        browser_ = CefBrowserHost::CreateBrowserSync(info, this,
            "", CefBrowserSettings(), nullptr);
        return !!browser_;
    }

    void close()
    {
        if (browser_)
            browser_->GetHost()->CloseBrowser(true);
        browser_ = nullptr;
    }

    void loadURL(const wchar_t * url)
    {
        if (!url)
            return;
        auto frame = mainFrame();
        if (!frame)
            return;
        frame->LoadURL(url);
    }

    void executeJS(const wchar_t * code, const wchar_t * url, int start_line)
    {
        if (!code)
            return;
        auto frame = mainFrame();
        if (!frame)
            return;
        frame->ExecuteJavaScript(code, url, start_line);
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
    //对mouse事件的转换
    uint32_t ToCefModifiers(UIEvent & e)
    {
        uint32_t modifiers = EVENTFLAG_NONE;
        if (e.isCapsLockOn())
            modifiers |= EVENTFLAG_CAPS_LOCK_ON;
        if (e.isShiftDown())
            modifiers |= EVENTFLAG_SHIFT_DOWN;
        if (e.isControlDown())
            modifiers |= EVENTFLAG_CONTROL_DOWN;
        if (e.isAltDown())
            modifiers |= EVENTFLAG_ALT_DOWN;
        if (e.isLeftMouseDown())
            modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
        if (e.isMiddleMouseDown())
            modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
        if (e.isRightMouseDown())
            modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;
        if (e.isCommandDown())
            modifiers |= EVENTFLAG_COMMAND_DOWN;
        if (e.isNumLockOn())
            modifiers |= EVENTFLAG_NUM_LOCK_ON;
        if (e.isKeyPad())
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        if (e.isLeft())
            modifiers |= EVENTFLAG_IS_LEFT;
        if (e.isRight())
            modifiers |= EVENTFLAG_IS_RIGHT;
        return modifiers;
    }

    CefMouseEvent ToCefMouseEvent(MouseEvent & e)
    {
        CefMouseEvent ce;
        ce.x = static_cast<int> (e.getLoc().x());
        ce.y = static_cast<int> (e.getLoc().y());
        ce.modifiers = ToCefModifiers(e);
        return ce;
    }

    CefKeyEvent ToCefKeyEvent(KeyEvent & e)
    {
        CefKeyEvent ce;
        ce.windows_key_code = e.key();
        ce.native_key_code = *(int *)(&e.state());
        ce.is_system_key = e.system();
        if (kET_KEY_DOWN == e.type())
            ce.type = KEYEVENT_RAWKEYDOWN;
        else if (kET_KEY_UP == e.type())
            ce.type = KEYEVENT_KEYUP;
        else
            ce.type = KEYEVENT_CHAR;

        ce.modifiers = ToCefModifiers(e);
        return ce;
    }

    void sendMouseMoveEvent(MouseEvent & e, bool leave)
    {
        CHECK_CEF_BROWSER_HOST;
        host->SendMouseMoveEvent(ToCefMouseEvent(e), leave);
    }

    void sendMouseClickEvent(MouseEvent & e)
    {
        CHECK_CEF_BROWSER_HOST;
        CefBrowserHost::MouseButtonType mt = MBT_LEFT;
        if (e.isMiddleMouse())
            mt = MBT_MIDDLE;
        else if (e.isRightMouse())
            mt = MBT_RIGHT;
        host->SendMouseClickEvent(ToCefMouseEvent(e), mt,
            e.type() == kET_MOUSE_UP, 0);
    }

    void sendFocusEvent(bool focus)
    {
        CHECK_CEF_BROWSER_HOST;
        host->SendFocusEvent(focus);
    }

    void sendMouseWheelEvent(WheelEvent & e)
    {
        CHECK_CEF_BROWSER_HOST;
        host->SendMouseWheelEvent(ToCefMouseEvent(e), e.dx(), e.dy());
    }

    void sendKeyEvent(KeyEvent & e)
    {
        CHECK_CEF_BROWSER_HOST;
        host->SendKeyEvent(ToCefKeyEvent(e));
    }
public:
    bool OnSetFocus(CefRefPtr<CefBrowser> browser,
        FocusSource source) 
    {
        web_view_->requestFocus();
        return true;
    }
    //CefLifeSpanHandler
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override
    {
        return this;
    }

    CefRefPtr<CefRenderHandler> GetRenderHandler()
    {
        return this;
    }

    CefRefPtr<CefFocusHandler> GetFocusHandler() 
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
        rect.x = rect.y = 0;
        rect.width = static_cast<int>(web_view_->getWidth());
        rect.height = static_cast<int>(web_view_->getHeight());
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
        web_view_->inval();
    }

    void OnCursorChange(CefRefPtr<CefBrowser> browser,
        CefCursorHandle cursor,
        CursorType type,
        const CefCursorInfo & custom_cursor_info) override
    {
        web_view_->setCursor(cursor);
    }
protected:
    CefFrame * mainFrame()
    {
        if (!browser_)
            return nullptr;
        return browser_->GetMainFrame();
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

bool WebView::StartUp(const char * locate)
{
    //不启用cef的话直接返回
    if (!Core::EnableCEF())
        return true;

    CefMainArgs args;
    CefSettings settings;
    settings.no_sandbox = true;
    settings.multi_threaded_message_loop = false;
    //settings.windowless_rendering_enabled = true;
    // Enable dev tools
    //settings.remote_debugging_port = 8088;
    // Use cefclient.exe to run render process and other sub processes.
    auto mod = Helper::GetModule();
    wchar_t mod_name[MAX_PATH];
    if (!::GetModuleFileName(mod, mod_name, MAX_PATH))
        return false;
    auto path = Helper::GetPathFromFullName(Encoding::ToUTF8(mod_name).data());
    const char * parts[] = { path.data(), "cefclient.exe" };
    auto clientpath = Helper::JoinPath(parts, sizeof(parts) / sizeof(parts[0]));

    CefString(&settings.browser_subprocess_path) = Encoding::FromUTF8(clientpath.data());
    // Load locales/zh-CN.pak
    if (!locate)
        locate = "zh-CN";
    CefString(&settings.locale) = locate;

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
:hack_focus_travel_(false)
{
    browser_ = new Browser(this);
    browser_->AddRef();
    //browser 居然new的时候没有引用计数+1
    assert(browser_->HasOneRef());
}

WebView::~WebView()
{
    close();
    browser_->Release();
}

bool WebView::open()
{
    return browser_->open();
}

void WebView::close()
{
    browser_->close();
}

void WebView::loadURL(const wchar_t * url)
{
    browser_->loadURL(url);
}

void WebView::executeJS(const wchar_t * code, const wchar_t * url, int start_line)
{
    browser_->executeJS(code, url, start_line);
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
    browser_->sendMouseMoveEvent(e, true);
}

void WebView::onMouseMove(MouseEvent & e)
{
    browser_->sendMouseMoveEvent(e, false);
}

void WebView::onMouseDown(MouseEvent & e)
{
    browser_->sendMouseClickEvent(e);
}

void WebView::onMouseUp(MouseEvent & e)
{
    browser_->sendMouseClickEvent(e);
}

void WebView::onWheel(WheelEvent & e)
{
    browser_->sendMouseWheelEvent(e);
}

void WebView::onFocus(FocusEvent & )
{
    hack_focus_travel_ = true;
    browser_->sendFocusEvent(true);
}

void WebView::onBlur(FocusEvent & e)
{
    browser_->sendFocusEvent(false);
}

void WebView::onKeyDown(KeyEvent & e)
{
    browser_->sendKeyEvent(e);
}

void WebView::onKeyUp(KeyEvent & e)
{
    browser_->sendKeyEvent(e);
}

void WebView::onChar(KeyEvent & e)
{//webview 输入框不接受tab字符
    if (e.ch() != '\t')
        browser_->sendKeyEvent(e);
}

bool WebView::skipDefaultKeyEventProcessing(const KeyEvent & e)
{//调用到这个函数时 必然当前焦点是webview
    return false;
}



}