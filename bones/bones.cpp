#include "bones.h"
#include "core/core_imp.h"
#include "core/font.h"

#include "binding/script_context.h"

static const wchar_t * third =
L"skia unknown\n"
L"rapidxml 1.13\n"
L"lua 5.3.0\n"
L"cef 3.2171.1979";

bones::ScriptParser * core = nullptr;

namespace bones
{
ScriptParser * GetCoreInstance()
{
    return core;
}

const char * kNotifyOrder = "__notify";
const char * kEventOrder = "__event";

const char * kMethodAddNotify = "addNotify";
const char * kMethodAddEvent = "addEvent";

const char * kMethodOnCreate = "onCreate";
const char * kMethodOnDestroy = "onDestroy";

const char * kMethodAnimateRun = "__run";
const char * kMethodAnimateStart = "__start";
const char * kMethodAnimateStop = "__stop";
const char * kMethodAnimatePause = "__pause";
const char * kMethodAnimateResume = "__resume";
const char * kMethodOnMouseDown = "onMouseDown";
const char * kMethodOnMouseUp = "onMouseUp";
const char * kMethodOnMouseMove = "onMouseMove";
const char * kMethodOnMouseLeave = "onMouseLeave";
const char * kMethodOnMouseEnter = "onMouseEnter";
const char * kMethodOnMouseClick = "onMouseClick";
const char * kMethodOnMouseDClick = "onMouseDClick";
const char * kMethodOnKeyDown = "onKeyDown";
const char * kMethodOnChar = "onChar";
const char * kMethodOnKeyUp = "onKeyUp";
const char * kMethodOnFocusIn = "onFocusIn";
const char * kMethodOnFocusOut = "onFocusOut";
const char * kMethodOnFocus = "onFocus";
const char * kMethodOnBlur = "onBlur";
const char * kMethodOnWheel = "onWheel";

const char * kMethodOnSizeChanged = "onSizeChanged";
const char * kMethodOnPositionChanged = "onPositionChanged";
const char * kMethodOnHitTest = "onHitTest";

const char * kMethodIndex = "__index";
const char * kMethodGC = "__gc";
const char * kMethodGetID = "getID";
const char * kMethodGetRoot = "getRoot";
const char * kMethodGetOpacity = "getOpacity";
const char * kMethodSetOpacity = "setOpacity";
const char * kMethodSetFocusable = "setFocusable";
const char * kMethodSetMouseable = "setMouseable";
const char * kMethodSetVisible = "setVisible";
const char * kMethodGetCObject = "getCObject";
const char * kMethodAnimate = "animate";
const char * kMethodStop = "stop";
const char * kMethodPause = "pause";
const char * kMethodResume = "resume";
const char * kMethodStopAll = "stopAll";

const char * kMethodContains = "contains";
const char * kMethodApplyCSS = "applyCSS";
const char * kMethodApplyClass = "applyClass";
const char * kMethodGetChildAt = "getChildAt";
const char * kMethodGetParent = "getParent";

const char * kMethodGetSize = "getSize";
const char * kMethodGetLoc = "getLoc";
const char * kMethodSetSize = "setSize";
const char * kMethodSetLoc = "setLoc";

const char * kMethodSetColor = "setColor";

const char * kStrCapturing = "capturing";
const char * kStrTarget = "target";
const char * kStrBubbling = "bubbling";

//const char * ToEventPhaseStr(BonesEvent::Phase phase)
//{
//    if (BonesEvent::kCapturing == phase)
//        return kStrCapturing;
//    if (BonesEvent::kBubbling == phase)
//        return kStrBubbling;
//    if (BonesEvent::kTarget == phase)
//        return kStrTarget;
//    return "";
//}

//Font ToFont(const BonesFont & font)
//{
//    Font ft;
//    ft.setFamily(font.family);
//    ft.setSize(font.size);
//    uint32_t style = Font::kNormal;
//    if (font.bold)
//        style |= Font::kBold;
//    if (font.italic)
//        style |= Font::kItalic;
//    if (font.underline)
//        style |= Font::kUnderline;
//    if (font.strike)
//        style |= Font::kStrikeOut;
//    ft.setStyle(style);
//    return ft;
//}

int EventStack::event_stack_count = 0;

EventStack::EventStack()
{ 
    event_stack_count++; 
}

EventStack::~EventStack()
{ 
    assert(event_stack_count); 
    event_stack_count--; 
}

int EventStack::getCount() const
{ 
    return event_stack_count; 
}

    

}

//BONES_API(BonesCore *) BonesStartUp(const BonesConfig & config)
//{
//    using namespace bones;
//    Core::Config cc;
//    cc.log_file = L".\\bones_dll.log";
//    cc.log_level = Log::kNone;
//    if (BonesConfig::kError == config.log_level)
//        cc.log_level = Log::kError;
//    else if (BonesConfig::kVerbose == config.log_level)
//        cc.log_level = Log::kVerbose;
//    cc.aa_enable = config.aa_enable;
//    cc.cef_enable = config.cef_enable;
//    cc.cef_locate = config.cef_locate;
//    bool bret = Core::StartUp(cc);
//    if (bret)
//        bret = LuaContext::StartUp();
//    if (bret)
//        core = new ScriptParser;
//
//    return core;
//}
//
//BONES_API(void) BonesShutDown()
//{
//    using namespace bones;
//    if (core)
//        delete core;
//    core = nullptr;
//    LuaContext::ShutDown();
//    Core::ShutDown();
//}
//
//BONES_API(void) BonesUpdate()
//{
//    using namespace bones;
//    Core::Update();
//}
//
//BONES_API(BonesCore *) BonesGetCore()
//{
//    return bones::GetCoreInstance();
//}


BONES_API(BonesContext *) BonesContextNew(const BonesConfig & config)
{
    return new bones::ScriptContext;
}

BONES_API(void) BonesContextTerm(BonesContext * ctx)
{
    if (!ctx)
        return;

    delete static_cast<bones::ScriptContext *>(ctx);
}




