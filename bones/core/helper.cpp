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

HBITMAP Helper::ToHBitmap(Surface & sf)
{
    return sf.hbm_;
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

}