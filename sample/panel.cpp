#include "panel.h"
#include <windowsx.h>
#include "utils.h"
#include <assert.h>

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
root_(nullptr), cursor_(NULL), dc_(NULL)
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

void BSPanel::loadRoot(const char * id)
{
    root_ = (BonesRoot *)BonesGetCore()->getObject(id);
    if (root_)
        root_->setListener(this);
}

void BSPanel::loadRichEdit(const char * id)
{
    ((BonesRichEdit *)BonesGetCore()->getObject(id))->setListener(this);
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
    cursor_ = ::LoadCursor(NULL, IDC_ARROW);
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
        ::SetCursor((HCURSOR)cursor_);
        return TRUE;
    }
    return defProcessEvent(uMsg, wParam, lParam);
}

LRESULT BSPanel::handleKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    root_->handleKey(uMsg, wParam, lParam);
    return 0;
}

LRESULT BSPanel::handleIME(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    root_->handleComposition(uMsg, wParam, lParam);
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
    root_->handleFocus(uMsg, wParam, lParam);
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

void BSPanel::requestFocus(BonesRoot * sender, bool & stop)
{
    ::SetFocus(hwnd_);
    stop = true;
}

void BSPanel::invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop)
{
    RECT r = { (LONG)rect.left, (LONG)rect.top, (LONG)rect.right, (LONG)rect.bottom };
    ::InvalidateRect(hwnd_, &r, TRUE);
    stop = true;
}

void BSPanel::changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop)
{
    if (cursor_ != cursor)
    {
        cursor_ = cursor;
        PostMessage(hwnd(), WM_SETCURSOR, 0, HTCLIENT);
    } 
    stop = true;
}

void BSPanel::showCaret(BonesRoot * sender, bool fshow, bool & stop)
{
    
    BOOL ret = 0;
    if (fshow)
        ret = ::ShowCaret(hwnd_);     
    else
        ret = ::HideCaret(hwnd_);
    stop = true;
}

void BSPanel::createCaret(BonesRoot * sender, BonesCaret hbmp, const BonesSize & size, bool & stop)
{
    
    auto ret = ::CreateCaret(hwnd_, hbmp, (int)size.width,  (int)size.height);
    stop = true;
}

void BSPanel::changeCaretPos(BonesRoot * sender, const BonesPoint & pt, bool & stop)
{
    stop = true;
    ::SetCaretPos((int)pt.x, (int)pt.y);
}

void BSPanel::onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop)
{
    ;
}
void BSPanel::onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop)
{
    ;
}

void BSPanel::onCreate(BonesRichEdit * sender, bool & stop)
{
    ;
}

void BSPanel::onDestroy(BonesRichEdit * sender, bool & stop)
{
    ;
}

HRESULT BSPanel::txNotify(BonesRichEdit * sender, DWORD iNotify, void  *pv, bool & stop)
{
    stop = true;
    return S_OK;
}

void BSPanel::onReturn(BonesRichEdit * sender, bool & stop)
{
    stop = true;
}

BOOL BSPanel::screenToClient(BonesRichEdit * sender, LPPOINT lppt, bool & stop)
{
    stop = true;
    return ::ScreenToClient(hwnd_, lppt);
}

BOOL BSPanel::clientToScreen(BonesRichEdit * sender, LPPOINT lppt, bool & stop)
{
    stop = true;
    return ::ClientToScreen(hwnd_, lppt);
}

HIMC BSPanel::immGetContext(BonesRichEdit * sender, bool & stop)
{
    stop = true;
    return ::ImmGetContext(hwnd_);
}

void BSPanel::immReleaseContext(BonesRichEdit * sender, HIMC himc, bool & stop)
{
    stop = true;
    ::ImmReleaseContext(hwnd_, himc);
}

