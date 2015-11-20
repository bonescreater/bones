#ifndef BONES_CORE_IMP_H_
#define BONES_CORE_IMP_H_

#include "core.h"
#include "color.h"
#include "logging.h"

class SkShader;
class SkPathEffect;

namespace bones
{

class AnimationManager;
class CSSManager;
class ResManager;
class XMLController;
class RootManager;
class Point;



class Core
{
public:
    struct Config
    {
        const wchar_t * log_file;
        Log::Level log_level;
        bool aa_enable;
        bool cef_enable;
        const char * cef_locate;//"zh-CN"
    };

    enum TileMode
    {
        kClamp = 0,
        kRepeat,
        kMirror,
    };
public:
    static bool StartUp(const Config & config);

    static void ShutDown();

    static void Update();

    static bool CEFEnable();

    static bool AntiAliasEnable();

    static AnimationManager * GetAnimationManager();

    static CSSManager * GetCSSManager();

    static ResManager * GetResManager();

    static RootManager * GetRootManager();

    static XMLController * GetXMLController();

    static SkShader * createLinearGradient(
        const Point & begin,
        const Point & end,
        size_t count,
        Color * color,
        Scalar * pos,
        TileMode mode);

    static SkShader * createRadialGradient(
        const Point & center,
        Scalar radius,
        size_t count,
        Color * color,
        Scalar * pos,
        TileMode mode);

    static void destroyShader(SkShader * shader);

    static SkPathEffect * createDashEffect(
        size_t count,
        Scalar * interval,
        Scalar offset);

    static void destroyEffect(SkPathEffect * effect);

    static SkPathEffect * getDashEffectCache();

};

}


#endif