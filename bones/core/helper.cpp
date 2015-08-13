#include "helper.h"
#include "rect.h"
#include "point.h"
#include "pixmap.h"
#include "shader.h"

#include "SkRect.h"
#include "SkBitmap.h"
#include "SkPixelRef.h"
#include <Dwmapi.h>

namespace bones
{


Rect Helper::ToRect(const RECT & wr)
{
    return Rect::MakeLTRB(static_cast<Scalar>(wr.left), 
                          static_cast<Scalar>(wr.top), 
                          static_cast<Scalar>(wr.right), 
                          static_cast<Scalar>(wr.bottom));
}

Point Helper::ToPoint(const POINT & wp)
{
    return Point::Make(static_cast<Scalar>(wp.x),
                       static_cast<Scalar>(wp.y));
}

SkRect Helper::ToSkRect(const Rect & rect)
{
    SkRect r;
    r.setLTRB(rect.left(), rect.top(), rect.right(), rect.bottom());
    return r;
}

SkIRect Helper::ToSkIRect(const Rect & rect)
{
    SkIRect r;
    r.setLTRB(static_cast<int>(rect.left()), 
              static_cast<int>(rect.top()),
              static_cast<int>(rect.right()),
              static_cast<int>(rect.bottom()));
    return r;
}

SkPoint Helper::ToSkPoint(const Point & pt)
{
    SkPoint p;
    p.set(pt.x(), pt.y());
    return p;
}

SkBitmap Helper::ToSkBitmap(Pixmap & pm)
{
    SkBitmap bitmap;
    SkBitmap subset;
    if (pm.isValid())
    {
        bitmap.setConfig(pm.pixel_ref_->info(), pm.pixel_ref_->rowBytes());
        bitmap.setPixelRef(pm.pixel_ref_);
        bitmap.extractSubset(&subset, ToSkIRect(pm.subset_));
    }
    return subset;
}

SkShader * Helper::ToSkShader(Shader & shader)
{
    return shader.getShader();
}

POINT Helper::ToPoint(const Point & pt)
{
    POINT wp;
    wp.x = static_cast<LONG>(pt.x());
    wp.y = static_cast<LONG>(pt.y());
    return wp;
}

RECT Helper::ToRect(const Rect & r)
{
    RECT wr;
    wr.left = static_cast<LONG>(r.left());
    wr.top = static_cast<LONG>(r.top());
    wr.right = static_cast<LONG>(r.right());
    wr.bottom = static_cast<LONG>(r.bottom());
    return wr;
}

HDC Helper::ToHDC(Surface & sf)
{
    if (sf.context_)
        return sf.context_->dc;
    return NULL;
}

HMODULE Helper::GetModuleFromAddress(void * address)
{

    DWORD flags = GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
    HMODULE module = 0;
    GetModuleHandleEx(flags, (LPCWSTR)address, &module);
    return module;
}

uint64_t Helper::GetTickCount()
{
    typedef ULONGLONG(WINAPI *TGetTickCount64)();
    typedef DWORD(WINAPI *TGetTickCount32)();

    static FARPROC tf = nullptr;
    static bool support_64 = false;
    if (!tf)
    {
        tf = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetTickCount64");
        support_64 = true;
        if (!tf)
            tf = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "GetTickCount");
            
        assert(tf);
    }
    if (!tf)
        return 0;
    return support_64 ? ((TGetTickCount64)tf)() : ((TGetTickCount32)tf)();
}

bool Helper::DWMEnabled()
{
    static HMODULE dwm_module = NULL;

    if (!dwm_module)
        dwm_module = ::GetModuleHandle(L"dwmapi.dll");
    if (!dwm_module)
        dwm_module = ::LoadLibrary(L"dwmapi.dll");
    if (!dwm_module)
        return false;

    BOOL result = TRUE;
    if (SUCCEEDED(::DwmIsCompositionEnabled(&result)))
        return !!result;
    return false;
}

bool Helper::ExtendFrameInfoClientArea(HWND hwnd,
    int left, int right, int top, int bottom)
{
    if (!DWMEnabled())
        return false;
    MARGINS margins = { left, right, top, bottom };
    return SUCCEEDED(::DwmExtendFrameIntoClientArea(hwnd, &margins));
}

Rect Helper::GetPrimaryWorkArea()
{
    Rect r;
    POINT pt = { 0, 0 };
    auto monitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    if (monitor)
    {
        MONITORINFO mi = { 0 };
        mi.cbSize = sizeof(mi);
        ::GetMonitorInfo(monitor, &mi);
        r.setLTRB(static_cast<float>(mi.rcWork.left),
            static_cast<float>(mi.rcWork.top),
            static_cast<float>(mi.rcWork.right),
            static_cast<float>(mi.rcWork.bottom));
    }
    return r;
}

WPARAM Helper::ToKeyStateForMouse(int flags)
{
    WPARAM wp = 0;

    if (flags & kEF_LEFT_MOUSE_DOWN)
        wp |= MK_LBUTTON;
    if (flags & kEF_RIGHT_MOUSE_DOWN)
        wp |= MK_RBUTTON;
    if (flags & kEF_MIDDLE_MOUSE_DOWN)
        wp |= MK_MBUTTON;
    if (flags & kEF_CONTROL_DOWN)
        wp |= MK_CONTROL;
    if (flags & kEF_SHIFT_DOWN)
        wp |= MK_SHIFT;
    return wp;
}

LPARAM Helper::ToCoordinateForMouse(const Point & pt)
{
    return MAKELPARAM(pt.x(), pt.y());
}

UINT Helper::ToMsgForMouse(EventType type, MouseButton button)
{
    if (kET_MOUSE_MOVE == type)
        return WM_MOUSEMOVE;
    if (kET_MOUSE_DOWN == type)
    {
        if (kMB_LEFT == button)
            return WM_LBUTTONDOWN;
        if (kMB_RIGHT == button)
            return WM_RBUTTONDOWN;
        if (kMB_MIDDLE == button)
            return WM_MBUTTONDOWN;
    }
    if (kET_MOUSE_UP == type)
    {
        if (kMB_LEFT == button)
            return WM_LBUTTONUP;
        if (kMB_RIGHT == button)
            return WM_RBUTTONUP;
        if (kMB_MIDDLE == button)
            return WM_MBUTTONUP;
    }
    if (kET_MOUSE_LEAVE == type)
        return WM_MOUSELEAVE;

    return WM_NULL;
}

LPARAM Helper::ToKeyStateForKey(const KeyState & state)
{
    return (LPARAM)*(uint32_t *)(&state);
}

int Helper::ToFlagsForEvent()
{
    int flags = kEF_NONE;
    if (::GetKeyState(VK_SHIFT) < 0)
        flags |= kEF_SHIFT_DOWN;
    if (::GetKeyState(VK_CONTROL) < 0)
        flags |= kEF_CONTROL_DOWN;
    if (::GetKeyState(VK_MENU) < 0)
        flags |= kEF_ALT_DOWN;
    if (::GetKeyState(VK_CAPITAL) < 0)
        flags |= kEF_CAPS_LOCK_DOWN;
    if (::GetKeyState(VK_LBUTTON) < 0)
        flags |= kEF_LEFT_MOUSE_DOWN;
    if (::GetKeyState(VK_RBUTTON) < 0)
        flags |= kEF_RIGHT_MOUSE_DOWN;
    if (::GetKeyState(VK_MBUTTON) < 0)
        flags |= kEF_MIDDLE_MOUSE_DOWN;
    if (::GetKeyState(kVKEY_COMMAND) < 0 || ::GetKeyState(kVKEY_RWIN) < 0)
        flags |= kEF_COMMAND_DOWN;

    return flags;
}

void Helper::ToEMForMouse(UINT msg, EventType & type, MouseButton & mb)
{
    type = kET_COUNT;
    mb = kMB_NONE;

    switch (msg)
    {
    case WM_MOUSEMOVE:
        type = kET_MOUSE_MOVE;
        break;
    case WM_LBUTTONDBLCLK:
    {
        type = kET_MOUSE_DCLICK;
        mb = kMB_LEFT;
    }
        break;
    case WM_LBUTTONDOWN:
    {
        type = kET_MOUSE_DOWN;
        mb = kMB_LEFT;
    }
        break;
    case WM_LBUTTONUP:
    {
        type = kET_MOUSE_UP;
        mb = kMB_LEFT;
    }
        break;
    case WM_RBUTTONDBLCLK:
    {
        type = kET_MOUSE_DCLICK;
        mb = kMB_RIGHT;
    }
        break;
    case WM_RBUTTONDOWN:
    {
        type = kET_MOUSE_DOWN;
        mb = kMB_RIGHT;
    }
        break;
    case WM_RBUTTONUP:
    {
        type = kET_MOUSE_UP;
        mb = kMB_RIGHT;
    }
        break;
    case WM_MOUSELEAVE:
        type = kET_MOUSE_LEAVE;
        break;
    default:
        assert(0);
        return;
    }
}

}