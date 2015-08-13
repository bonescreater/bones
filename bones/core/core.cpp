#include "core.h"
#include "panel.h"
#include "logging.h"
#include "animation_manager.h"
#include "panel_manager.h"
#include "css_manager.h"

#include "helper.h"

#include "SkGraphics.h"

namespace bones
{
const char * kClassRef = "Ref";
const char * kClassWidget = "Widget";
const char * kClassPanel = "Panel";
const char * kClassRootView = "RootView";
const char * kClassView = "View";
const char * kClassArea = "Area";
const char * kClassImage = "Image";
const char * kClassShape = "Shape";
const char * kClassText = "Text";
const char * kClassRichEdit = "RichEdit";
const char * kClassWebView = "WebView";

const char * kClassAnimation = "Animation";


static AnimationManager * animations = nullptr;

static PanelManager * panels = nullptr;

static CSSManager * css = nullptr;

bool Core::StartUp(const Config & config)
{
    SkGraphics::Init();
    bool bret = Panel::Initialize();
    if (bret)
        bret = Log::StartUp(config.log_file, config.log_level);
    if (bret)
        bret = !!(animations = new AnimationManager);
    if (bret)
        bret = !!(panels = new PanelManager);
    if (bret)
        bret = !!(css = new CSSManager);

    return bret;
}

void Core::ShutDown()
{
    if (panels)
        delete panels;
    panels = nullptr;
    if (animations)
        delete animations;
    animations = nullptr;

    Log::ShutDown();
    Panel::Uninitialize();
    SkGraphics::Term();
}

void Core::Update()
{
    static uint64_t current = 0;
    if (0 == current)
        current = Helper::GetTickCount();

    //
    //防止死循环
    auto frame_count = Helper::GetTickCount();
    auto delta = frame_count - current;
    if (delta >= 1)
    {
        if (animations)
            animations->run(delta);
        current = frame_count;
    }
    //window 绘制
    panels->update();
}

AnimationManager * Core::GetAnimationManager()
{
    return animations;
}

PanelManager * Core::GetPanelManager()
{
    return panels;
}

CSSManager * Core::GetCSSManager()
{
    return css;
}

}