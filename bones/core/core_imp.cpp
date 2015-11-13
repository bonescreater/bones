#include "core_imp.h"
#include "logging.h"
#include "animation_manager.h"
#include "css_manager.h"
#include "res_manager.h"
#include "root_manager.h"

#include "xml_controller.h"
#include "point.h"

#include "web_view.h"
#include "helper.h"

#include "SkGraphics.h"
#include "SkGradientShader.h"
#include "SkDashPathEffect.h"

namespace bones
{
//跟xml标签一样为了方便
const char * kClassRoot = "root";
const char * kClassView = "view";
const char * kClassImage = "image";
const char * kClassShape = "shape";
const char * kClassText = "text";
const char * kClassInput = "input";
const char * kClassRichEdit = "richedit";
const char * kClassWebView = "webview";
const char * kClassScroller = "scroller";

static SkShader::TileMode ToSkShaderTileMode(Core::TileMode mode)
{
    SkShader::TileMode sm = SkShader::kClamp_TileMode;
    if (Core::kRepeat == mode)
        sm = SkShader::kRepeat_TileMode;
    else if (Core::kMirror == mode)
        sm = SkShader::kMirror_TileMode;
    return sm;
}

static AnimationManager * animations = nullptr;

static CSSManager * css = nullptr;

static ResManager * res = nullptr;

static RootManager * roots = nullptr;

static XMLController * xml = nullptr;

static bool cef_enable = false;

static bool aa_enable = false;


bool Core::StartUp(const Config & config)
{
    SkGraphics::Init();
    aa_enable = config.aa_enable;
    bool bret = Log::StartUp(config.log_file, config.log_level);
    if (bret)
        bret = !!(animations = new AnimationManager);
    if (bret)
        bret = !!(css = new CSSManager);
    if (bret)
        bret = !!(res = new ResManager);
    if (bret)
        bret = !!(roots = new RootManager);
    if (bret)
    {
        cef_enable = config.cef_enable;
        bret = WebView::StartUp(config.cef_locate);
    }
    if (bret)
        bret = !!(xml = new XMLController);

    return bret;
}

void Core::ShutDown()
{
    if (xml)
        delete xml;
    xml = nullptr;

    WebView::ShutDown();

    if (roots)
        delete roots;
    roots = nullptr;

    if (res)
        delete res;
    res = nullptr;

    if (css)
        delete css;
    css = nullptr;

    if (animations)
        delete animations;
    animations = nullptr;

    Log::ShutDown();
    SkGraphics::Term();
}

void Core::Update()
{
    static uint64_t current = 0;
    if (0 == current)
        current = Helper::GetTickCount();

    auto frame_count = Helper::GetTickCount();
    auto delta = frame_count - current;
    if (delta >= 1)
    {
        if (animations)
            animations->run(delta);
        current = frame_count;
    }
    WebView::Update();
    roots->update();
}

bool Core::CEFEnable()
{
    return cef_enable;
}

bool Core::AntiAliasEnable()
{
    return aa_enable;
}

AnimationManager * Core::GetAnimationManager()
{
    return animations;
}

CSSManager * Core::GetCSSManager()
{
    return css;
}

ResManager * Core::GetResManager()
{
    return res;
}

RootManager * Core::GetRootManager()
{
    return roots;
}

XMLController * Core::GetXMLController()
{
    return xml;
}

SkShader * Core::createLinearGradient(
    const Point & begin,
    const Point & end,
    size_t count,
    Color * color,
    Scalar * pos,
    TileMode mode)
{
    SkPoint pt[2] = { { begin.x(), begin.y() }, { end.x(), end.y() } };
    return SkGradientShader::CreateLinear(pt, color, pos, count,
        ToSkShaderTileMode(mode));
}

SkShader * Core::createRadialGradient(
    const Point & center,
    Scalar radius,
    size_t count,
    Color * color,
    Scalar * pos,
    TileMode mode)
{
    SkPoint pt = { center.x(), center.y() };

    return SkGradientShader::CreateRadial(pt, radius, color, pos, count,
        ToSkShaderTileMode(mode));
}

void Core::destroyShader(SkShader * shader)
{
    if (shader)
        shader->unref();
}

SkPathEffect * Core::createDashEffect(
    size_t count,
    Scalar * interval,
    Scalar offset)
{
    if (interval && count && ((count % 2) == 0))
        return SkDashPathEffect::Create(interval, count, offset);
    else
    {
        Scalar interval[2] = { 2, 2 };
        return SkDashPathEffect::Create(interval, 2, 0);
    }
}

void Core::destroyEffect(SkPathEffect * effect)
{
    if (effect)
        effect->unref();
}

}