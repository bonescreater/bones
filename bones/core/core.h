#ifndef BONES_CORE_H_
#define BONES_CORE_H_


#include <stdint.h>
#include <memory.h>
#include <assert.h>
#include <functional>

#include <Windows.h>


class SkPathEffect;
namespace bones
{
extern const char * kClassRoot;
extern const char * kClassView;
extern const char * kClassImage;
extern const char * kClassShape;
extern const char * kClassText;
extern const char * kClassRichEdit;
extern const char * kClassWebView;
extern const char * kClassScroller;

typedef float Scalar;
typedef HANDLE Cursor;
typedef HBITMAP Caret;
typedef HWND Widget;

class AnimationManager;
class CSSManager;
class ResManager;
class XMLController;

#define BONES_WHEEL_SPEED (WHEEL_DELTA / 4.0f)

enum LogLevel
{
    kLOG_LEVEL_NONE = 0,
    kLOG_LEVEL_ERROR = 1,// only error
    kLOG_LEVEL_VERBOSE = 2,// everything
};

class Core
{
public:
    struct Config
    {
        const wchar_t * log_file;
        LogLevel log_level;
        bool cef_enable;
        const char * cef_locate;//"zh-CN"
    };
public:
    static bool StartUp(const Config & config);

    static void ShutDown();

    static void Update();

    static bool CEFEnable();

    static AnimationManager * GetAnimationManager();

    static CSSManager * GetCSSManager();

    static ResManager * GetResManager();

    static SkPathEffect * GetDashEffect();

    static XMLController * GetXMLController();
};

}
#endif