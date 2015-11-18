#include "helper.h"
#include "rect.h"
#include "point.h"
#include "pixmap.h"
#include "font.h"
#include "core_imp.h"
#include "SkRect.h"
#include "SkBitmap.h"
#include "SkPixelRef.h"
#include "SkPaint.h"
#include "SkTypeface.h"


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

void Helper::ToSkPaint(const Font & font, SkPaint & paint)
{
    auto style = font.getStyle();
    paint.setTextSize(font.getSize());
    paint.setTextEncoding(SkPaint::kUTF16_TextEncoding);
    paint.setAntiAlias(Core::AntiAliasEnable());
    //paint.setDistanceFieldTextTEMP(true);
    paint.setUnderlineText(!!(style & Font::kUnderline));
    paint.setStrikeThruText(!!(style & Font::kStrikeOut));

    bool bold = !!(Font::kBold & style);
    bool italic = !!(Font::kItalic & style);

    SkTypeface::Style st = SkTypeface::kNormal;
    if (bold && italic)
        st = SkTypeface::kBoldItalic;
    else if (bold)
        st = SkTypeface::kBold;
    else if (italic)
        st = SkTypeface::kItalic;
        
    SkTypeface * type_face = SkTypeface::CreateFromName(font.getFamily(), st);
    if (type_face)
    {
        paint.setTypeface(type_face);
        type_face->unref();
    }
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

HBITMAP Helper::ToHBITMAP(const Surface & sf)
{
    if (sf.hbm_)
        return sf.hbm_;
    return NULL;
}

HMODULE Helper::GetModule()
{

    DWORD flags = GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT |
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS;
    HMODULE module = 0;
    GetModuleHandleEx(flags, (LPCWSTR)_ReturnAddress(), &module);
    return module;
}

std::string Helper::JoinPath(const char ** path, int count)
{
    if (!path || !count)
        return "";
    std::string paths;
    for (auto i = 0; i < count; ++i)
    {
        if (nullptr == path[i])
            break;

        if (0 == i)
        {
            paths = path[0];
            continue;
        }
        //如果path 末尾不是斜杆 那么加1个斜杆
        char last_char = paths[paths.length() - 1];
        if ('\\' != last_char || '/' != last_char)
            paths.append("\\");
        paths.append(path[i]);
    }
    return paths;
}

std::string Helper::GetPathFromFullName(const char * fullname)
{
    std::string path;
    auto last_backslash = std::strrchr(fullname, '\\');
    auto last_slash = std::strrchr(fullname, '/');
    auto slash = last_backslash > last_slash ? last_backslash : last_slash;
    if (!slash)
        return path;
    return path.assign(fullname, slash - fullname);
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
    return (LPARAM)state.state;
}




}