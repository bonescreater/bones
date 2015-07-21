#include "core.h"
#include "panel.h"
#include "logging.h"
#include "animation_manager.h"
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
const char * kClassBlock = "Block";
const char * kClassShape = "Shape";
const char * kClassText = "Text";
const char * kClassRichEdit = "RichEdit";
const char * kClassWebView = "WebView";

const char * kClassCommonAnimation = "CommonAnimation";


static AnimationManager * animations = nullptr;

bool Core::StartUp(const Config & config)
{
    SkGraphics::Init();
    bool bret = Panel::Initialize();
    if (bret)
        bret = Log::StartUp(config.log_file, config.log_level);
    if (bret)
        animations = new AnimationManager;

    return bret;
}

void Core::ShutDown()
{
    if (animations)
        delete animations;

    Log::ShutDown();
    Panel::Uninitialize();
    SkGraphics::Term();
}

void Core::Update()
{
    static uint64_t current = 0;
    if (0 == current)
        current = Helper::GetTickCount();

    //防止死循环
    auto frame_count = Helper::GetTickCount();
    auto delta = frame_count - current;
    if (delta < 1)
        return;

    if (animations)
        animations->run(delta);
    current = frame_count;
}

AnimationManager * Core::GetAnimationManager()
{
    return animations;
}

}