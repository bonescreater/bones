#include "panel.h"
#include <windowsx.h>
#include "utils.h"
#include <assert.h>
#include <vector>

static const wchar_t * kClassName = L"BSPanel";



static LRESULT CALLBACK BSPanelProc(HWND hWnd,
                                    UINT uMsg,
                                    WPARAM wParam,
                                    LPARAM lParam)
{
    if (WM_NCCREATE == uMsg && ::GetWindowLongPtr(hWnd, GWLP_USERDATA) == 0)
    {
        LPCREATESTRUCT lcp = reinterpret_cast<LPCREATESTRUCT>(lParam);
        BSPanel * panel = reinterpret_cast<BSPanel *>(lcp->lpCreateParams);
        panel->attach(hWnd);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(panel));
    }

    LONG user_data = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    auto sheet = reinterpret_cast<BSPanel *>(user_data);
    if (sheet)
        return sheet->processEvent(uMsg, wParam, lParam);

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

static HMODULE GetModuleFromAddress(void * address)
{

    DWORD flags = GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
    HMODULE module = 0;
    GetModuleHandleEx(flags, (LPCWSTR)address, &module);
    return module;
}
//from cef osr
static int ToFlagsForKeyEvent(WPARAM wparam, LPARAM lparam)
{
    int flags = BonesRoot::kMFNone;
    if (::GetKeyState(VK_SHIFT) < 0)
        flags |= BonesRoot::kMFShiftDown;
    if (::GetKeyState(VK_CONTROL) < 0)
        flags |= BonesRoot::kMFControlDown;
    if (::GetKeyState(VK_MENU) < 0)
        flags |= BonesRoot::kMFAltDown;
    // Low bit set from GetKeyState indicates "toggled".
    if (::GetKeyState(VK_NUMLOCK) & 1)
        flags |= BonesRoot::kMFNumLockOn;
    if (::GetKeyState(VK_CAPITAL) & 1)
        flags |= BonesRoot::kMFCapsLockOn;

    switch (wparam) {
    case VK_RETURN:
        if ((lparam >> 16) & KF_EXTENDED)
            flags |= BonesRoot::kMFIsKeyPad;
        break;
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
        if (!((lparam >> 16) & KF_EXTENDED))
            flags |= BonesRoot::kMFIsKeyPad;
        break;
    case VK_NUMLOCK:
    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
    case VK_DIVIDE:
    case VK_MULTIPLY:
    case VK_SUBTRACT:
    case VK_ADD:
    case VK_DECIMAL:
    case VK_CLEAR:
        flags |= BonesRoot::kMFIsKeyPad;
        break;
    case VK_SHIFT:
        if (::GetKeyState(VK_LSHIFT))
            flags |= BonesRoot::kMFIsLeft;
        else if (::GetKeyState(VK_RSHIFT))
            flags |= BonesRoot::kMFIsRight;
        break;
    case VK_CONTROL:
        if (::GetKeyState(VK_LCONTROL))
            flags |= BonesRoot::kMFIsLeft;
        else if (::GetKeyState(VK_RCONTROL))
            flags |= BonesRoot::kMFIsRight;
        break;
    case VK_MENU:
        if (::GetKeyState(VK_LMENU))
            flags |= BonesRoot::kMFIsLeft;
        else if (::GetKeyState(VK_RMENU))
            flags |= BonesRoot::kMFIsRight;
        break;
    case VK_LWIN:
        flags |= BonesRoot::kMFIsLeft;
        break;
    case VK_RWIN:
        flags |= BonesRoot::kMFIsRight;
        break;
    }
    return flags;
}

static int ToFlagsForMouseEvent()
{
    int flags = BonesRoot::kMFNone;;
    if (::GetKeyState(VK_SHIFT) < 0)
        flags |= BonesRoot::kMFShiftDown;
    if (::GetKeyState(VK_CONTROL) < 0)
        flags |= BonesRoot::kMFControlDown;
    if (::GetKeyState(VK_MENU) < 0)
        flags |= BonesRoot::kMFAltDown;

    if (::GetKeyState(VK_LBUTTON) < 0)
        flags |= BonesRoot::kMFLeftMouseDown;
    if (::GetKeyState(VK_RBUTTON) < 0)
        flags |= BonesRoot::kMFRightMouseDowm;
    if (::GetKeyState(VK_MBUTTON) < 0)
        flags |= BonesRoot::kMFMiddleMouseDown;

    // Low bit set from GetKeyState indicates "toggled".
    if (::GetKeyState(VK_NUMLOCK) & 1)
        flags |= BonesRoot::kMFNumLockOn;
    if (::GetKeyState(VK_CAPITAL) & 1)
        flags |= BonesRoot::kMFCapsLockOn;
    return flags;
}

bool BSPanel::Initialize()
{
    HMODULE module = GetModuleFromAddress(&BSPanelProc);
    WNDCLASSEX wndcls = { 0 };
    wndcls.cbSize = sizeof(wndcls);
    wndcls.style = CS_HREDRAW | CS_VREDRAW;
    wndcls.lpfnWndProc = BSPanelProc;
    wndcls.cbClsExtra = 0;
    wndcls.cbWndExtra = 0;
    wndcls.hInstance = module;
    wndcls.hIcon = 0;
    wndcls.hCursor = ::LoadCursor(NULL, IDC_HELP);
    wndcls.hbrBackground = 0;
    wndcls.lpszMenuName = 0;
    wndcls.lpszClassName = kClassName;
    wndcls.hIconSm = 0;
    return !!::RegisterClassEx(&wndcls);
}

bool BSPanel::Uninitialize()
{
    HMODULE module = GetModuleFromAddress(&BSPanelProc);
    return !!::UnregisterClass(kClassName, module);
}

BSPanel::BSPanel() :track_mouse_(false), ex_style_(0), hwnd_(NULL),
root_(nullptr), cursor_(kBonesArrow), dc_(NULL), ime_start_(false)
{
    ;
}

BSPanel::~BSPanel()
{
    ;
}

bool BSPanel::create(const BSPanel * parent, bool layered)
{
    DWORD ex_style = WS_EX_APPWINDOW;
    DWORD win_style = WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
    if (layered)
    {
        ex_style |= WS_EX_LAYERED;
        win_style = WS_POPUP;
    }
        

    auto hwnd = ::CreateWindowEx(ex_style, kClassName, L"mmm", win_style,
        0, 0, 0, 0, NULL, 0, 0, this);
    return !!hwnd;
}

bool BSPanel::destroy()
{
    bool result = false;
    if (hwnd_ != NULL && hwnd_ != HWND_BROADCAST)
    {
        result = !!::DestroyWindow(hwnd_);
    }
    return result;
}

void BSPanel::attach(HWND hwnd)
{
    hwnd_ = hwnd;
}

void BSPanel::detach()
{
    hwnd_ = NULL;
}

HWND BSPanel::hwnd() const
{
    return hwnd_;
}

bool BSPanel::setLoc(const POINT & pt)
{
    uint32_t flag = SWP_NOSIZE | SWP_NOZORDER |
        SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;

    return !!::SetWindowPos(hwnd_, 0, pt.x, pt.y, 0, 0, flag);
}

bool BSPanel::setSize(const SIZE & size)
{
    uint32_t flag = SWP_NOMOVE | SWP_NOZORDER |
        SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;

    return !!::SetWindowPos(hwnd_, 0, 0, 0,
        static_cast<int>(size.cx),
        static_cast<int>(size.cy), flag);
}

bool BSPanel::isLayered() const
{
    return !!(ex_style_ & WS_EX_LAYERED);
}

void BSPanel::loadRoot(BonesRoot * root)
{
    root_ = root;
    if (root_)
    {
        root_->setListener(this);
        root_->attachTo(hwnd());
    }
        
}

void BSPanel::layeredDraw()
{
    if (!isLayered())
        return;
    if (!root_)
        return;
    if (!root_->isDirty())
        return;
    root_->draw();

    RECT win_rect;
    ::GetWindowRect(hwnd_, &win_rect);
    POINT pos = { win_rect.left, win_rect.top };
    SIZE size = { win_rect.right - win_rect.left, 
                  win_rect.bottom - win_rect.top };
    POINT src_pos = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    auto old = ::SelectObject(dc_, root_->getBackBuffer());
    ::SetGraphicsMode(dc_, GM_ADVANCED);
    auto l = ::UpdateLayeredWindow(hwnd(), NULL, &pos, &size, dc_, &src_pos, 0, &bf, ULW_ALPHA);
    ::SelectObject(dc_, old);
}

LRESULT BSPanel::handleCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ex_style_ = ::GetWindowLongPtr(hwnd_, GWL_EXSTYLE);
    dc_ = ::CreateCompatibleDC(NULL);
    return 0;
}

LRESULT BSPanel::handleDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    detach();
    if (root_)
        root_->setListener(nullptr);
    root_ = nullptr;
    ::DeleteDC(dc_);
    ::PostQuitMessage(0);
    return 0;
}

LRESULT BSPanel::handleSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (HTCLIENT == LOWORD(lParam))
    {
        if (kBonesHandle == cursor_)
        {
            ::SetCursor((HCURSOR)cursor_content_);
            return TRUE;
        }

        auto wc = IDC_ARROW;
        if (cursor_ == kBonesIbeam)
            wc = IDC_IBEAM;
        else if (cursor_ == kBonesWait)
            wc = IDC_WAIT;
        else if (cursor_ == kBonesCross)
            wc = IDC_CROSS;
        else if (cursor_ == kBonesUpArrow)
            wc = IDC_UPARROW;
        else if (cursor_ == kBonesSize)
            wc = IDC_SIZE;
        else if (cursor_ == kBonesIcon)
            wc = IDC_ICON;
        else if (cursor_ == kBonesSizeNWSE)
            wc = IDC_SIZENWSE;
        else if (cursor_ == kBonesSizeNESW)
            wc = IDC_SIZENESW;
        else if (cursor_ == kBonesSizeWE)
            wc = IDC_SIZEWE;
        else if (cursor_ == kBonesSizeNS)
            wc = IDC_SIZENS;
        else if (cursor_ == kBonesSizeAll)
            wc = IDC_SIZEALL;
        else if (cursor_ == kBonesNo)
            wc = IDC_NO;
        else if (cursor_ == kBonesHand)
            wc = IDC_HAND;
        else if (cursor_ == kBonesAppStarting)
            wc = IDC_APPSTARTING;
        else if (cursor_ == kBonesHelp)
            wc = IDC_HELP;

        ::SetCursor((HCURSOR)::LoadImage(NULL, wc, IMAGE_CURSOR, 0, 0, LR_SHARED));
        return TRUE;
    }
    return defProcessEvent(uMsg, wParam, lParam);
}

LRESULT BSPanel::handleKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BonesRoot::KeyMessage km;
    if (WM_CHAR == uMsg)
        km = BonesRoot::kChar;
    else if (WM_SYSCHAR == uMsg)
        km = BonesRoot::kSysChar;
    else if (WM_KEYDOWN == uMsg)
        km = BonesRoot::kKeyDown;
    else if (WM_SYSKEYDOWN == uMsg)
        km = BonesRoot::kSysKeyDown;
    else if (WM_KEYUP == uMsg)
        km = BonesRoot::kKeyUp;
    else if (WM_SYSKEYUP == uMsg)
        km = BonesRoot::kSysKeyUp;
    else
        return 0;

    root_->sendKey(km, wParam, lParam, ToFlagsForKeyEvent(wParam, lParam));
    return 0;
}

LRESULT BSPanel::handleIME(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    BonesRoot::IMEMessage m;
    BonesRoot::IMEInfo info = { 0 };
    std::vector<wchar_t> str;
    str.resize(1);
    if (WM_IME_STARTCOMPOSITION == uMsg)
    {
        ime_start_ = true;
        m = BonesRoot::kCompositionStart;
    }
    else if (WM_IME_COMPOSITION == uMsg)
    {
        m = BonesRoot::kCompositionUpdate;
        info.dbcs = wParam;
        HIMC imc = ::ImmGetContext(hwnd_);
        if (imc)
        {
            info.index = lParam;
            if (lParam & GCS_RESULTSTR)
            {
                auto size = ImmGetCompositionString(imc, GCS_RESULTSTR, &str[0], 0);
                str.resize(size / 2 + 1);
                ImmGetCompositionString(imc, GCS_RESULTSTR, &str[0], size + 2);
                str[size / 2] = 0;                        
            }
            else if (lParam & GCS_COMPSTR)
            {  
                if (lParam & GCS_CURSORPOS)
                    info.cursor = ImmGetCompositionString(imc, GCS_CURSORPOS, NULL, 0);

                auto size = ImmGetCompositionString(imc, GCS_COMPSTR, &str[0], 0);
                str.resize(size / 2 + 1);
                ImmGetCompositionString(imc, GCS_COMPSTR, &str[0], size + 2);
                str[size / 2] = 0;
            }
            info.str = &str[0];
            ::ImmReleaseContext(hwnd_, imc);
        }

    }     
    else if (WM_IME_ENDCOMPOSITION == uMsg)
    {
        m = BonesRoot::kCompositionEnd;
        ime_start_ = false;
    }
    else
        return 0;
    //cef osr 目前不支持ime 临时通过返回值来处理
    if (!root_->sendComposition(m, BonesRoot::kCompositionUpdate == m ? &info : nullptr))
        return defProcessEvent(uMsg, wParam, lParam);
    return 0;
}

LRESULT BSPanel::handlePositionChanges(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_WINDOWPOSCHANGED:
        onGeometryChanged(*(WINDOWPOS *)lParam);
        break;
    }
    return 0;
}

LRESULT BSPanel::handleFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    root_->sendFocus(uMsg == WM_SETFOCUS);
    return 0;
}

LRESULT BSPanel::handlePaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(hwnd_, &ps);
    onPaint(ps.hdc, ps.rcPaint);
    ::EndPaint(hwnd_, &ps);
    return 0;
}

LRESULT BSPanel::handleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
        BonesPoint p = { static_cast<BonesScalar>(pt.x), static_cast<BonesScalar>(pt.y) };
        root_->sendWheel(WM_MOUSEHWHEEL == uMsg ? delta : 0, 
            WM_MOUSEWHEEL == uMsg ? delta : 0, p, ToFlagsForMouseEvent());
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
    BonesRoot::MouseMessage mm;
    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        mm = BonesRoot::kMouseMove;
        break;
    case WM_LBUTTONDBLCLK:
        mm = BonesRoot::kLButtonDClick;
        break;
    case WM_LBUTTONDOWN:
        mm = BonesRoot::kLButtonDown;
        break;
    case WM_LBUTTONUP:
        mm = BonesRoot::kLButtonUp;
        break;
    case WM_RBUTTONDBLCLK:
        mm = BonesRoot::kRButtonDClick;
        break;
    case WM_RBUTTONDOWN:
        mm = BonesRoot::kRButtonDown;
        break;
    case WM_RBUTTONUP:
        mm = BonesRoot::kRButtonUp;
        break;
    case WM_MBUTTONDOWN:
        mm = BonesRoot::kMButtonDown;
        break;
    case WM_MBUTTONUP:
        mm = BonesRoot::kMButtonUp;
        break;
    case WM_MBUTTONDBLCLK:
        mm = BonesRoot::kMButtonDClick;
        break;
    case WM_MOUSELEAVE:
        mm = BonesRoot::kMouseLeave;
        break;
    default:
        return 0;
    }
    BonesPoint p = { static_cast<BonesScalar>(GET_X_LPARAM(lParam)),
        static_cast<BonesScalar>(GET_Y_LPARAM(lParam)) };
    root_->sendMouse(mm, p, ToFlagsForMouseEvent());
    return 0;
}

LRESULT BSPanel::processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST ||
        uMsg == WM_MOUSEHOVER || uMsg == WM_MOUSELEAVE)
    {
        return handleMouse(uMsg, wParam, lParam);
    }
    else
    {
        switch (uMsg)
        {
        case WM_CREATE:
            return handleCreate(uMsg, wParam, lParam);
        case WM_DESTROY:
            return handleDestroy(uMsg, wParam, lParam);
        case WM_WINDOWPOSCHANGED:
            return handlePositionChanges(uMsg, wParam, lParam);
        case WM_PAINT:
            return handlePaint(uMsg, wParam, lParam);
        case WM_SETCURSOR:
            return handleSetCursor(uMsg, wParam, lParam);
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            return handleFocus(uMsg, wParam, lParam);
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_CHAR:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SYSCHAR:
            return handleKey(uMsg, wParam, lParam);
        case WM_IME_STARTCOMPOSITION:
        case WM_IME_COMPOSITION:
        case WM_IME_ENDCOMPOSITION:
            return handleIME(uMsg, wParam, lParam);
        default:
            return defProcessEvent(uMsg, wParam, lParam);
        }
    }
}

void BSPanel::onGeometryChanged(WINDOWPOS & pos)
{
    if (root_)
    {
        BSStr s;
        RECT client;
        ::GetClientRect(hwnd_, &client);
        s << "{";
        s << "height: " << client.bottom - client.top << "px;";
        s << "width:" << client.right - client.left << "px;";
        s << "}";
        root_->applyCSS(s.str().data());
    }
}

void BSPanel::onPaint(HDC hdc, const RECT & rect)
{
    //layered window 没有onPaint消息 不通过onPaint绘制
    if (isLayered())
        return;
    if (!root_)
        return;

    if (root_->isDirty())
        root_->draw();

    //只更新脏区
    auto old = ::SelectObject(dc_, root_->getBackBuffer());
    ::BitBlt(hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
        dc_, rect.left, rect.top, SRCCOPY);
    ::SelectObject(dc_, old);
}

LRESULT BSPanel::defProcessEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hwnd_, uMsg, wParam, lParam);
}

void BSPanel::onCreate(BonesRoot * sender, bool & stop)
{
    ;
}

void BSPanel::onDestroy(BonesRoot * sender, bool & stop)
{
    ;
}

void BSPanel::onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop)
{
    ;
}

void BSPanel::onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop)
{
    ;
}

bool BSPanel::onHitTest(BonesRoot * sender, const BonesPoint & loc, bool & stop)
{
    return true;
}

void BSPanel::requestFocus(BonesRoot * sender, bool & stop)
{
    ::SetFocus(hwnd_);
    stop = true;
}

void BSPanel::shiftFocus(BonesRoot * sender, BonesObject * prev,
    BonesObject * current, bool & stop)
{
    if (!ime_start_)
        return;
    auto imc = ImmGetContext(hwnd_);
    if (imc)
    {
        ImmNotifyIME(imc, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
        ImmReleaseContext(hwnd_, imc);
    }
    assert(!ime_start_);
}

void BSPanel::invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop)
{
    RECT r = { (LONG)rect.left, (LONG)rect.top, 
               (LONG)(rect.right + 1), 
               (LONG)(rect.bottom + 1)};
    ::InvalidateRect(hwnd_, &r, TRUE);
    stop = true;
}

void BSPanel::changeCursor(BonesRoot * sender, BonesCursor cursor, 
    void * content, bool & stop)
{
    cursor_ = cursor;
    cursor_content_ = content;

    stop = true;
}

void BSPanel::changeCaretPos(BonesRoot * sender, const BonesPoint & point, bool &stop)
{
    if (ime_start_)
    {
        auto imc = ImmGetContext(hwnd_);
        if (imc)
        {
            COMPOSITIONFORM composition;
            composition.dwStyle = CFS_POINT;
            composition.ptCurrentPos.x = static_cast<LONG>(point.x);
            composition.ptCurrentPos.y = static_cast<LONG>(point.y);
            ImmSetCompositionWindow(imc, &composition);

            ImmReleaseContext(hwnd_, imc);
        }
    }
    stop = true;
}


