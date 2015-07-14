#ifndef BONES_CORE_HELPER_H_
#define BONES_CORE_HELPER_H_

#include "core.h"

struct SkRect;
struct SkIRect;
class SkBitmap;

namespace bones
{

class Pixmap;
class Rect;

class Helper
{
public:
    static SkRect ToSkRect(const Rect & rect);

    static SkIRect ToSkIRect(const Rect & rect);

    static SkBitmap ToSkBitmap(Pixmap & pm);

    static HMODULE GetModuleFromAddress(void * address);

    static uint64_t GetTickCount();

    static bool DWMSupport();

    static bool DWMEnabled();

    static bool ExtendFrameInfoClientArea(HWND hwnd,
        int left, int right, int top, int bottom);

    static Rect GetPrimaryWorkArea();
};

}
#endif