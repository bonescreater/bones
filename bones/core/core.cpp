#include "core.h"
#include "Panel.h"
#include "logging.h"
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
    Log::ShutDown();
    Panel::Uninitialize();
    SkGraphics::Term();
}

}