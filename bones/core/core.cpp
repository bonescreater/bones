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

const char * kClassAnimation = "Animation";

bool Core::StartUp(const Config & config)
{
    SkGraphics::Init();
    bool bret = Panel::Initialize();
    if (bret)
        bret = Log::StartUp(config.log_file, config.log_level);
    
    return bret;
}

void Core::ShutDown()
{
    AnimationManager::Get()->removeAll();

    Log::ShutDown();
    Panel::Uninitialize();
    SkGraphics::Term();
}

void Core::Update()
{
    static uint64_t current = 0;
    if (current = 0)
        current = Helper::GetTickCount();

    //防止死循环
    auto frame_count = Helper::GetTickCount();
    auto delta = frame_count - current;
    if (delta < 1)
        return;

    AnimationManager::Get()->run(delta);
    current = frame_count;
}

}