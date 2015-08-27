#ifndef BONES_CORE_HELPER_H_
#define BONES_CORE_HELPER_H_

#include "core.h"
#include "event.h"

struct SkRect;
struct SkIRect;
struct SkPoint;
class SkBitmap;
class SkShader;
class SkPaint;

namespace bones
{

class Pixmap;
class Surface;
class Rect;
class Point;
class Shader;
struct KeyState;
class Font;

class Helper
{
public:
    static Rect ToRect(const RECT & wr);

    static Point ToPoint(const POINT & wp);

    static SkRect ToSkRect(const Rect & rect);

    static SkIRect ToSkIRect(const Rect & rect);

    static SkPoint ToSkPoint(const Point & pt);

    static SkBitmap ToSkBitmap(Pixmap & pm);

    static SkShader * ToSkShader(Shader & shader);

    static void ToSkPaint(const Font & font, SkPaint & pt);

    static POINT ToPoint(const Point & pt);

    static RECT ToRect(const Rect & r);

    static HDC ToHDC(Surface & sf);

    static HMODULE GetModuleFromAddress(void * address);

    static uint64_t GetTickCount();

    static WPARAM ToKeyStateForMouse(int flags);

    static LPARAM ToCoordinateForMouse(const Point & pt);

    static UINT ToMsgForMouse(EventType type, MouseButton button);

    static LPARAM ToKeyStateForKey(const KeyState & state);

    static int ToFlagsForEvent();

    static void ToEMForMouse(UINT msg, EventType & type, MouseButton & mb);
};

}
#endif