#include "panel.h"
#include "helper.h"

namespace bones
{



static const wchar_t * kClassName = L"SheetWin";


static LRESULT CALLBACK PanelProc(HWND hWnd,
                                     UINT uMsg,
                                     WPARAM wParam,
                                     LPARAM lParam)
{
    if (WM_NCCREATE == uMsg && ::GetWindowLongPtr(hWnd, GWLP_USERDATA) == 0)
    {
        LPCREATESTRUCT lcp = reinterpret_cast<LPCREATESTRUCT>(lParam);
        Panel * panel = reinterpret_cast<Panel *>(lcp->lpCreateParams);
        panel->attach(hWnd);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(panel));
    }

    LONG user_data = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
    auto sheet = reinterpret_cast<Panel *>(user_data);
    if (sheet)
        return sheet->processEvent(uMsg, wParam, lParam);

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool Panel::Initialize()
{
    HMODULE module = Helper::GetModuleFromAddress(&PanelProc);
    WNDCLASSEX wndcls = { 0 };
    wndcls.cbSize = sizeof(wndcls);
    wndcls.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wndcls.lpfnWndProc = PanelProc;
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

bool Panel::Uninitialize()
{
    HMODULE module = Helper::GetModuleFromAddress(&PanelProc);
    return !!::UnregisterClass(kClassName, module);
}

Panel::Panel() :track_mouse_(false), ex_style_(0)
{
    ;
}

bool Panel::create(const Panel * parent)
{
    DWORD ex_style = 0;
    if (!parent)//owner窗口
        ex_style = WS_EX_APPWINDOW;

    auto hwnd = ::CreateWindowEx(ex_style, kClassName, L"",
        (WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
        0, 0, 0, 0, parent ? parent->hwnd() : NULL, 0, 0, this);
    layered_dc_ = CreateCompatibleDC(NULL);
    Helper::ExtendFrameInfoClientArea(hwnd, -1, -1, -1, -1);
    cursor_ = ::LoadCursor(NULL, IDC_ARROW);
    ex_style_ = ::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    
    return !!hwnd;
}

bool Panel::destroy()
{
    bool result = false;
    if (isValid() && hwnd_ != HWND_BROADCAST)
    {
        result = !!::DestroyWindow(hwnd_);
    }
    return result;
}

const char * Panel::getClassName() const
{
    return kClassPanel;
}

RootView * Panel::getRootView() const
{
    return root_.get();
}

bool Panel::isLayered() const
{
    return !!(ex_style_ & WS_EX_LAYERED);
}

void Panel::update()
{
    if (!isLayered())
        return;
    if (!root_)
        return;
    if (!root_->isDirty())
        return;

    root_->draw();
    auto & pm = root_->getBackBuffer();
    if (!pm.isValid() || pm.isEmpty())
        return;

    Pixmap::LockRec lr;
    if (!pm.lock(lr))
        return;


    Rect win_rect;
    getWindowRect(win_rect);
    POINT pos = { (LONG)win_rect.left(), (LONG)win_rect.top() };
    SIZE size = { (LONG)win_rect.width(), (LONG)win_rect.height() };
    POINT src_pos = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    
    auto old = ::SelectObject(layered_dc_, Helper::ToHBitmap(pm));
    auto r = ::UpdateLayeredWindow(hwnd(), NULL, &pos, &size, layered_dc_, &src_pos, 0, &bf, ULW_ALPHA);
    ::SelectObject(layered_dc_, old);

    pm.unlock();
}

void Panel::setNCArea(NCArea area, const Rect & rect)
{
    if (area >= kNCAreaCount)
        return;
    nc_area_[area] = rect;
}

void Panel::setCursor(Cursor cursor)
{
    cursor_ = cursor;
}

void Panel::setColor(Color color)
{
    root_->setColor(color);
}

void Panel::setOpacity(float opacity)
{
    root_->setOpacity(opacity);
}

void Panel::setEXStyle(uint64_t ex_style)
{
    ::SetWindowLongPtr(hwnd(), GWL_EXSTYLE, (LONG)ex_style);
    ex_style_ = ::GetWindowLongPtr(hwnd(), GWL_EXSTYLE);
}

float Panel::getOpacity() const
{
    return root_->getOpacity();
}

LRESULT Panel::handleNCCreate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    root_ = AdoptRef(new RootView);
    root_->setDelegate(this);
    return 1;
}

LRESULT Panel::handleNCDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam)
{//如果父Panel先destroy 那么在子pannel destroy之前 windows已经发送了WM_DESTROY消息给子pannel
    root_->recursiveDetachChildren();   
    //如果在窗口鼠标事件处理过程中 进行destroy  windows不会 发送WM_MOUSELEAVE消息
    //所以需要手动发一个 告诉root view 鼠标离开 不然root view的mouse over始终指向自己
    //导致root view 释放不了
    MouseEvent e(kET_MOUSE_LEAVE, kMB_NONE, root_.get(), Point(), Point(), 0);
    root_->handleEvent(e);
    ::DeleteDC(layered_dc_);
    detach();
    return 0;
}

LRESULT Panel::handleSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ::SetCursor((HCURSOR)cursor_);
    return TRUE;
}

LRESULT Panel::handleNCCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam == TRUE)
        return 0;
    else
        return defProcessEvent(uMsg, wParam, lParam);
}

LRESULT Panel::handlePositionChanges(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_WINDOWPOSCHANGING:
        onGeometryChanging(*(WINDOWPOS *)lParam);
        break;
    case WM_WINDOWPOSCHANGED:
        onGeometryChanged(*(WINDOWPOS *)lParam);
        break;
    }
    return 0;
}

LRESULT Panel::handleFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return defProcessEvent(uMsg, wParam, lParam);
}

LRESULT Panel::handleNCHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    ::ScreenToClient(hwnd(), &pt);
    float x = static_cast<float>(pt.x);
    float y = static_cast<float>(pt.y);

    int i = 0;
    for (; i < kNCAreaCount; ++i)
    {
        auto & rect = nc_area_[i];
        if (!rect.isEmpty() && rect.contains(x, y))
            break;
    }
    switch (i)
    {
    case kCaption:
        return HTCAPTION;
    case kBottom:
        return HTBOTTOM;
    case kBottomLeft:
        return HTBOTTOMLEFT;
    case kBottomRight:
        return HTBOTTOMRIGHT;
    case kLeft:
        return HTBOTTOMLEFT;
    case kRight:
        return HTBOTTOMRIGHT;
    case kTop:
        return HTTOP;
    case kTopLeft:
        return HTTOPLEFT;
    case kTopRight:
        return HTTOPRIGHT;
    default:
        return defProcessEvent(uMsg, wParam, lParam);
        break;
    }

}

LRESULT Panel::handlePaint(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(hwnd_, &ps);

    Rect rect;
    float left = static_cast<float>(ps.rcPaint.left);
    float top = static_cast<float>(ps.rcPaint.top);
    float right = static_cast<float>(ps.rcPaint.right);
    float bottom = static_cast<float>(ps.rcPaint.bottom);
    rect.setLTRB(left, top, right, bottom);
    onPaint(ps.hdc, rect);

    ::EndPaint(hwnd_, &ps);
    return 0;
}

LRESULT Panel::handleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int flags = kEF_NONE;
    if (wParam & MK_LBUTTON)
        flags |= kEF_LEFT_MOUSE_DOWN;
    if (wParam & MK_RBUTTON)
        flags |= kEF_RIGHT_MOUSE_DOWN;
    if (wParam & MK_MBUTTON)
        flags |= kEF_MIDDLE_MOUSE_DOWN;
    if (wParam & MK_CONTROL)
        flags |= kEF_CONTROL_DOWN;
    if (wParam & MK_SHIFT)
        flags |= kEF_SHIFT_DOWN;
    //暂时不管XBUTTON
    EventType et = kET_COUNT;
    MouseButton mb = kMB_NONE;

    if (!track_mouse_)
    {
        TRACKMOUSEEVENT track_event =
        {
            sizeof(track_event),
            TME_LEAVE,
            hwnd(),
            HOVER_DEFAULT
        };
        track_mouse_ = ::TrackMouseEvent(&track_event) != FALSE;
    }

    if (WM_MOUSEWHEEL == uMsg || WM_MOUSEHWHEEL == uMsg)
    {
        et = kET_MOUSE_WHEEL;
        auto delta = GET_WHEEL_DELTA_WPARAM(wParam);
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);

        ::ScreenToClient(hwnd(), &pt);
        Point p(static_cast<float>(pt.x), static_cast<float>(pt.y));

        WheelEvent e(et, root_.get(), WM_MOUSEHWHEEL == uMsg ? delta : 0,
            WM_MOUSEWHEEL == uMsg ? delta : 0, p, p, flags);
        root_->handleEvent(e);
        return 0;
    }

    switch (uMsg)
    {
    case WM_MOUSEMOVE:
        et = kET_MOUSE_MOVE;
        break;
    case WM_LBUTTONDBLCLK:
    {
        et = kET_MOUSE_DCLICK;
        mb = kMB_LEFT;
    }
        break;
    case WM_LBUTTONDOWN:
    {
        ::SetCapture(hwnd());
        et = kET_MOUSE_DOWN;
        mb = kMB_LEFT;
    }
        break;
    case WM_LBUTTONUP:
    {
        ::ReleaseCapture();
        et = kET_MOUSE_UP;
        mb = kMB_LEFT;
    }
        break;
    case WM_RBUTTONDBLCLK:
    {
        et = kET_MOUSE_DCLICK;
        mb = kMB_RIGHT;
    }
        break;
    case WM_RBUTTONDOWN:
    {
        et = kET_MOUSE_DOWN;
        mb = kMB_RIGHT;
    }
        break;
    case WM_RBUTTONUP:
    {
        et = kET_MOUSE_UP;
        mb = kMB_RIGHT;
    }
        break;
    case WM_MOUSELEAVE:
    {
        et = kET_MOUSE_LEAVE;
        track_mouse_ = false;
    }
        break;
    default:
        assert(0);
        return 0;
    }
    Point p(static_cast<float>(GET_X_LPARAM(lParam)), 
            static_cast<float>(GET_Y_LPARAM(lParam)));
    MouseEvent e(et, mb, root_.get(), p, p, flags);
    root_->handleEvent(e);
    return 0;
}

LRESULT Panel::processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
        case WM_NCCREATE:
            return handleNCCreate(uMsg, wParam, lParam);
        case WM_NCDESTROY:
            return handleNCDestroy(uMsg, wParam, lParam);
        case WM_NCCALCSIZE:
            return handleNCCalcSize(uMsg, wParam, lParam);
        case WM_NCHITTEST:
            return handleNCHitTest(uMsg, wParam, lParam);
        case WM_WINDOWPOSCHANGED:
        case WM_WINDOWPOSCHANGING:
            return handlePositionChanges(uMsg, wParam, lParam);
        case WM_PAINT:
            return handlePaint(uMsg, wParam, lParam);
        case WM_SETCURSOR:
            return handleSetCursor(uMsg, wParam, lParam);
        default:
            return defProcessEvent(uMsg, wParam, lParam);
        }
    }
}

void Panel::onGeometryChanged(WINDOWPOS & pos)
{
    root_->setSize(Size::Make(static_cast<Scalar>(pos.cx), static_cast<Scalar>(pos.cy)));
}

void Panel::onGeometryChanging(WINDOWPOS & pos)
{
    ;
}

void Panel::onPaint(HDC hdc, const Rect & rect)
{
    //layered window 没有onPaint消息 不通过onPaint绘制
    if (isLayered())
        return;

    Rect dirty_rect = root_->getDirtyRect();
    if (root_->isDirty())
        root_->draw();

    dirty_rect.join(rect);
    if (dirty_rect.isEmpty())
        return;

    //只更新脏区
    Pixmap pm = root_->getBackBuffer().extractSubset(dirty_rect);
    if (!pm.isValid() || pm.isEmpty())
        return;

    Pixmap::LockRec lr;
    if (!pm.lock(lr))
        return;

    int bm_width = static_cast<int>(root_->getWidth());
    int bm_height = static_cast<int>(root_->getHeight());

    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm_width;
    bmi.bmiHeader.biHeight = -bm_height; // top-down image
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = 0;

    //
    // Do the SetDIBitsToDevice.
    //
    // TODO(wjmaclean):
    //       Fix this call to handle SkBitmaps that have rowBytes != width,
    //       i.e. may have padding at the end of lines. The SkASSERT below
    //       may be ignored by builds, and the only obviously safe option
    //       seems to be to copy the bitmap to a temporary (contiguous)
    //       buffer before passing to SetDIBitsToDevice().
    //SkASSERT(bitmap.width() * bitmap.bytesPerPixel() == bitmap.rowBytes());
    int iRet = SetDIBitsToDevice(
        hdc,
        static_cast<int>(lr.subset.left()),
        static_cast<int>(lr.subset.top()),

        static_cast<int>(lr.subset.width()),
        static_cast<int>(lr.subset.height()),
        static_cast<int>(lr.subset.left()),
        static_cast<int>(bm_height - lr.subset.bottom()),
        0, bm_height,
        lr.bits,
        &bmi,
        DIB_RGB_COLORS);

    pm.unlock();
}

LRESULT Panel::defProcessEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::DefWindowProc(hwnd_, uMsg, wParam, lParam);
}

void Panel::invalidateRect(const Rect & rect)
{
    //layered windows 无效
    if (isLayered())
        return;
    RECT inval;
    inval.left = static_cast<int>(rect.left());
    inval.top = static_cast<int>(rect.top());
    inval.right = static_cast<int>(rect.right());
    inval.bottom = static_cast<int>(rect.bottom());

    ::InvalidateRect(hwnd(), &inval, TRUE);
}

void Panel::changeCursor(Cursor cursor)
{
    cursor_ = cursor;
}

}