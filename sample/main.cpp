
#include "bones/bones.h"
#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include <sstream>
#include <windowsx.h>

//bool TestCB(BonesCObject co, BonesArg * arg, size_t arg_count, void * userdata)
//{
//    BonesUnregScriptCallback(co, "onClick");
//    return true;
//}
//
//bool TestClose(BonesCObject co, BonesArg * arg, size_t arg_count, void * userdata)
//{
//    BonesCleanXML();
//    ::PostQuitMessage(0);
//    return true;
//}

//int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
class TestWindow : public BonesRoot::Listener
{
public:
    static bool Initialize();

    static bool Uninitialize();

    HWND create();

    void attach(BonesRoot * root)
    {
        root_ = root;
        root_->setListener(this);
    }

    void attach(HWND hwnd)
    {
        hwnd_ = hwnd;
    }

    LRESULT processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void requestFocus(BonesRoot * sender, bool & stop) override
    {
        ::SetFocus(hwnd_);
        stop = true;
    }

    void invalidRect(BonesRoot * sender, BonesRect & rect, bool & stop) override
    {
        RECT r = { rect.left, rect.top, rect.right, rect.bottom };
        ::InvalidateRect(hwnd_, &r, TRUE);
        stop = true;
    }

    void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) override
    {
        cursor_ = cursor;
        stop = true;
    }
    void onSizeChanged(BonesRoot * sender, BonesSize size, bool & stop) override
    {
        ;
    }
    void onPositionChanged(BonesRoot * sender, BonesPoint loc, bool & stop) override
    {
        ;
    }
private:
    HWND hwnd_;
    BonesRoot * root_;
    BonesCursor cursor_;
    bool track_mouse_;
};

TestWindow test_window;

class LoadListener : public BonesXMLListener
{
public:
    bool onLoad(BonesCore *) override
    { 
        test_window.attach((BonesRoot *)BonesGetCore()->getObject("main"));
        return true;
    }
};

int main()
{
    //创建窗口
    TestWindow::Initialize();

    auto hwnd = test_window.create();

    BonesConfig config;
    config.log_file = L"./test.log";
    config.log_level = kBONES_LOG_LEVEL_VERBOSE;

    BonesStartUp(config);

    auto f = fopen("..\\..\\sample\\test.xml", "rb");
    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    char * xml = (char *)malloc(len + 1);
    fseek(f, 0, SEEK_SET);
    fread(xml, 1, len, f);
    fclose(f);
    xml[len] = '\0';
    LoadListener load;
    BonesGetCore()->loadXMLString(xml, &load);
    free(xml);

     ::SetWindowPos(hwnd, 0, 0, 0, 800, 600, SWP_NOMOVE | SWP_NOZORDER |
        SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
     ::ShowWindow(hwnd, SW_NORMAL);
     ::UpdateWindow(hwnd);

    //BonesCleanXML();
    auto close_btn = BonesGetCore()->getObject("close");
    if (close_btn)
        close_btn->listen("onClick", nullptr);
        

    //消息循环
    MSG msg;
    while (1)
    {
        if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message != WM_QUIT)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                
                break;
            }                
        }
        else
        {
            BonesUpdate();
            ::SleepEx(5, true);
        }

    }
    BonesShutDown();
    TestWindow::Uninitialize();
    //getchar();
    return 0;
}

static const wchar_t * kClassName = L"TestWindow";

static LRESULT CALLBACK TestWindowProc(HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);

static HMODULE GetModuleFromAddress(void * address)
{

    DWORD flags = GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
    HMODULE module = 0;
    GetModuleHandleEx(flags, (LPCWSTR)address, &module);
    return module;
}

bool TestWindow::Initialize()
{
    HMODULE module = GetModuleFromAddress(&TestWindowProc);
    WNDCLASSEX wndcls = { 0 };
    wndcls.cbSize = sizeof(wndcls);
    wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndcls.lpfnWndProc = TestWindowProc;
    wndcls.cbClsExtra = 0;
    wndcls.cbWndExtra = 0;
    wndcls.hInstance = module;
    wndcls.hIcon = 0;
    wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wndcls.hbrBackground = 0;
    wndcls.lpszMenuName = 0;
    wndcls.lpszClassName = kClassName;
    wndcls.hIconSm = 0;
    return !!::RegisterClassEx(&wndcls);
}

bool TestWindow::Uninitialize()
{
    HMODULE module = GetModuleFromAddress(&TestWindowProc);
    return !!::UnregisterClass(kClassName, module);
}

HWND TestWindow::create()
{
    DWORD ex_style = 0;

    auto hwnd = ::CreateWindowEx(WS_EX_APPWINDOW, kClassName, L"",
        (WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU),
        0, 0, 0, 0,  NULL, 0, 0, this);
    cursor_ = ::LoadCursor(NULL, IDC_ARROW);
    track_mouse_ = false;
    return hwnd;
}

LRESULT CALLBACK TestWindowProc(HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    if (WM_NCCREATE == uMsg && ::GetWindowLongPtr(hWnd, GWLP_USERDATA) == 0)
    {
        LPCREATESTRUCT lcp = reinterpret_cast<LPCREATESTRUCT>(lParam);
        TestWindow * tw = reinterpret_cast<TestWindow *>(lcp->lpCreateParams);
        tw->attach(hWnd);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(tw));
    }

    LONG user_data = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    auto tw = reinterpret_cast<TestWindow *>(user_data);
    if (tw)
        return tw->processEvent(uMsg, wParam, lParam);

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

std::ostringstream s;
LRESULT TestWindow::processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool handle = true;

    if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ||
        uMsg == WM_MOUSEHOVER || uMsg == WM_MOUSELEAVE)
    {
        if (!track_mouse_)
        {
            TRACKMOUSEEVENT track_event =
            {
                sizeof(track_event),
                TME_LEAVE,
                hwnd_,
                HOVER_DEFAULT
            };
            track_mouse_ = ::TrackMouseEvent(&track_event) != FALSE;
        }
        if (WM_MOUSEWHEEL == uMsg || WM_MOUSEHWHEEL == uMsg)
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ::ScreenToClient(hwnd_, &pt);
            root_->handleWheel(uMsg, wParam, MAKELPARAM(pt.x, pt.y));
            return 0;
        }
        switch (uMsg)
        {
        case WM_LBUTTONDOWN:
            ::SetCapture(hwnd_);
            break;
        case WM_LBUTTONUP:
            ::ReleaseCapture();
            break;
        case WM_MOUSELEAVE:
            track_mouse_ = false;
            break;
        }
        root_->handleMouse(uMsg, wParam, lParam);
        return 0;
    }
    else
    {
        switch (uMsg)
        {
        case WM_WINDOWPOSCHANGED:
        {
            RECT client;
            ::GetClientRect(hwnd_, &client);
            s.clear();
            s << "{";
            s << "height: " << client.bottom - client.top << "px;";
            s << "width:" << client.right - client.left << "px;";
            s << "}";
            root_->applyCSS(s.str().data());
        }
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT ps = { 0 };
            ::BeginPaint(hwnd_, &ps);

            auto & rect = ps.rcPaint;
            
            if (root_->isDirty())
                root_->draw();

            ::BitBlt(ps.hdc,
                rect.left, rect.top,
                rect.right - rect.left,
                rect.bottom - rect.top,
                root_->dc(), rect.left, rect.top, SRCCOPY);

            ::EndPaint(hwnd_, &ps);
            return 0;
        }
            break;
        case WM_SETCURSOR:
        {
            if (HTCLIENT == LOWORD(lParam))
            {
                ::SetCursor((HCURSOR)cursor_);
                return TRUE;
            }
            else
                handle = false;
        }
            break;
        case WM_DESTROY:
            BonesGetCore()->cleanXML();
            ::PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        {
            root_->handleKey(uMsg, wParam, lParam);
            return 0;
        }
            break;
        case WM_CREATE:
        
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            //return handleFocus(uMsg, wParam, lParam);

            //return handleKey(uMsg, wParam, lParam);
        case WM_IME_STARTCOMPOSITION:
        case WM_IME_COMPOSITION:
        case WM_IME_ENDCOMPOSITION:
            //return handleIME(uMsg, wParam, lParam);
        default:
            handle = false;
        }
    }
    if (!handle)
        return ::DefWindowProc(hwnd_, uMsg, wParam, lParam);
    return 0;
}