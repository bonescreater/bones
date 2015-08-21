#include "core.h"
#include "logging.h"
#include "animation_manager.h"
#include "css_manager.h"

#include "helper.h"

#include "SkGraphics.h"

namespace bones
{
const char * kClassRef = "Ref";
const char * kClassRoot = "Root";
const char * kClassView = "View";
const char * kClassArea = "Area";
const char * kClassImage = "Image";
const char * kClassShape = "Shape";
const char * kClassText = "Text";
const char * kClassRichEdit = "RichEdit";
const char * kClassWebView = "WebView";


static AnimationManager * animations = nullptr;

static CSSManager * css = nullptr;

bool Core::StartUp(const Config & config)
{
    SkGraphics::Init();
    bool bret = Log::StartUp(config.log_file, config.log_level);
    if (bret)
        bret = !!(animations = new AnimationManager);
    if (bret)
        bret = !!(css = new CSSManager);

    return bret;
}

void Core::ShutDown()
{
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
}

AnimationManager * Core::GetAnimationManager()
{
    return animations;
}

CSSManager * Core::GetCSSManager()
{
    return css;
}

}