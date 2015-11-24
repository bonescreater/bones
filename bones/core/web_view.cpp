#include "web_view.h"
#include "core_imp.h"
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

    View::Cursor ToBonesCursor(CursorType cef_cursor)
    {
        View::Cursor cursor = View::kArrow;
        if (CT_CROSS == cef_cursor)
            cursor = View::kCross;
        else if (CT_HAND == cef_cursor)
            cursor = View::kHand;
        else if (CT_IBEAM == cef_cursor)
            cursor = View::kIbeam;
        else if (CT_WAIT == cef_cursor)
            cursor = View::kWait;
        else if (CT_HELP == cef_cursor)
            cursor = View::kHelp;
        else if (CT_EASTRESIZE == cef_cursor)
            cursor = View::kSizeWE;
        else if (CT_NORTHRESIZE == cef_cursor)
            cursor = View::kSizeNS;
        else if (CT_NORTHEASTRESIZE == cef_cursor)
            cursor = View::kSizeNESW;
        else if (CT_NORTHWESTRESIZE == cef_cursor)
            cursor = View::kSizeNWSE;
        else if (CT_SOUTHRESIZE == cef_cursor)
            cursor = View::kSizeNS;
        else if (CT_SOUTHEASTRESIZE == cef_cursor)
            cursor = View::kSizeNWSE;
        else if (CT_SOUTHWESTRESIZE == cef_cursor)
            cursor = View::kSizeNESW;
        else if (CT_WESTRESIZE == cef_cursor)
            cursor = View::kSizeWE;
        else if (CT_NORTHSOUTHRESIZE == cef_cursor)
            cursor = View::kSizeNS;
        else if (CT_EASTWESTRESIZE == cef_cursor)
            cursor = View::kSizeWE;
        else if (CT_NORTHEASTSOUTHWESTRESIZE == cef_cursor)
            cursor = View::kSizeNESW;
        else if (CT_NORTHWESTSOUTHEASTRESIZE == cef_cursor)
            cursor = View::kSizeNWSE;
        else if (CT_NONE == cef_cursor)
            cursor = View::kNo;
        else if (CT_CUSTOM == cef_cursor)
            cursor = View::kHandle;

        return cursor;

    }
public:
    Browser()
    {
        ;
    }

    void setHost(WebView * wv)
    {
        web_view_ = wv;
    }

    bool open()
    {
        CefWindowInfo info;
        info.SetAsWindowless(NULL, true);

        info.x = 0;
        info.y = 0;
        info.width = static_cast<int>(web_view_ ? web_view_->getWidth() : 0);
        info.height = static_cast<int>(web_view_ ? web_view_->getHeight(): 0);
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
    {//cef cursor是异步的 可能在我们鼠标已经离开webview后 webview还会发送鼠标样式
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
        web_view_ ? web_view_->requestFocus() : 0;
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
        rect.width = static_cast<int>(web_view_ ? web_view_->getWidth() : 0);
        rect.height = static_cast<int>(web_view_ ? web_view_->getHeight():0);
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
    {//先简单转换下 webview的光标样式

        auto vc = ToBonesCursor(type);
        web_view_ ? web_view_->setCursor(vc, View::kHandle == vc ? cursor : nullptr) : 0;
    }
protected:
    CefRefPtr<CefFrame> mainFrame()
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
    if (!Core::CEFEnable())
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
    wchar_t mod_name[MAX_PATH] = { 0 };
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
    if (!Core::CEFEnable())
        return;
    CefShutdown();
}

void WebView::Update()
{
    if (!Core::CEFEnable())
        return;
    CefDoMessageLoopWork();
}

WebView::WebView()
:hack_focus_travel_(false), delegate_(nullptr)
{
    browser_ = new Browser();
    browser_->setHost(this);
    browser_->AddRef();
    //browser 居然new的时候没有引用计数+1
    assert(browser_->HasOneRef());
}

WebView::~WebView()
{
    close();
    browser_->setHost(nullptr);
    browser_->Release();
}

void WebView::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
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

WebView::DelegateBase * WebView::delegate()
{
    return delegate_;
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
    Area::onSizeChanged();
}

void WebView::onMouseLeave(MouseEvent & e)
{
    Area::onMouseLeave(e);
    if (e.canceled())
        return;
    if (Event::kTarget != e.phase())
        return;
    browser_->sendMouseMoveEvent(e, true);
}

void WebView::onMouseMove(MouseEvent & e)
{
    Area::onMouseMove(e);
    if (e.canceled())
        return;
    if (Event::kTarget != e.phase())
        return;
    browser_->sendMouseMoveEvent(e, false);
}

void WebView::onMouseDown(MouseEvent & e)
{
    Area::onMouseDown(e);
    if (e.canceled())
        return;
    if (Event::kTarget != e.phase())
        return;
    browser_->sendMouseClickEvent(e);
}

void WebView::onMouseUp(MouseEvent & e)
{
    Area::onMouseUp(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    browser_->sendMouseClickEvent(e);
}

void WebView::onWheel(WheelEvent & e)
{
    Area::onWheel(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    browser_->sendMouseWheelEvent(e);
}

void WebView::onFocus(FocusEvent & e)
{
    Area::onFocus(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    hack_focus_travel_ = true;
    browser_->sendFocusEvent(true);
}

void WebView::onBlur(FocusEvent & e)
{
    Area::onBlur(e);
    if (e.canceled())
        return;
    if (Event::kTarget != e.phase())
        return;
    browser_->sendFocusEvent(false);
}

void WebView::onKeyDown(KeyEvent & e)
{
    Area::onKeyDown(e);
    if (e.canceled())
        return;
    if (Event::kTarget != e.phase())
        return;
    browser_->sendKeyEvent(e);
}

void WebView::onKeyUp(KeyEvent & e)
{
    Area::onKeyUp(e);
    if (e.canceled())
        return;

    if (Event::kTarget != e.phase())
        return;
    browser_->sendKeyEvent(e);
}

void WebView::onChar(KeyEvent & e)
{//webview 输入框不接受tab字符
    Area::onChar(e);
    if (e.canceled())
        return;
    if (Event::kTarget != e.phase())
        return;
    if (e.ch() != '\t')
        browser_->sendKeyEvent(e);
}

void WebView::onCompositionUpdate(CompositionEvent & e)
{
    auto index = e.index();
    if (CompositionEvent::kResultStr & index)
    {//插入结果字符串
        auto str = e.str();
        if (!str)
            return;
        for (size_t i = 0; i < wcslen(str); ++i)
        {
            KeyState state;
            state.state = 0;
            KeyEvent ke(kET_CHAR, this, static_cast<KeyboardCode>(str[i]), state, false, 0);
            onChar(ke);
        }
    }
}

bool WebView::skipDefaultKeyEventProcessing(const KeyEvent & e)
{//调用到这个函数时 必然当前焦点是webview
    return false;
}



}