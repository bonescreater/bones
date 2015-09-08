#include "bones.h"
#include "script_parser.h"
#include "lua_context.h"
//#include "core/pixmap.h"
//#include "core/css_manager.h"
//
//namespace bones
//{
//
////const char * kStrETMouseEnter = "et-mouse-enter";
////const char * kStrETMouseMove = "et-mouse-move";
////const char * kStrETMouseDown = "et-mouse-down";
////const char * kStrETMouseUp = "et-mouse-up";
////const char * kStrETMouseClick = "et-mouse-click";
////const char * kStrETMouseDClick = "et-mouse-dclick";
////const char * kStrETMouseLeave = "et-mouse-leave";
////const char * kStrETMouseWHEEL = "et-mouse-wheel";
////const char * kStrETKeyDown = "et-key-down";
////const char * kStrETKeyUp = "et-key-up";
////const char * kStrETFocusOut = "et-focus-out";
////const char * kStrETFocusIn = "et-focus-in";
////const char * kStrETBlur = "et-blur";
////const char * kStrETFocus = "et-focus";
////const char * kStrETCustom = "et-custom";
////const char * kStrETUnknown = "et-unknown";
//
//}
//

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
const char * kMethodOnKeyPress = "onKeyPress";
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
const char * kMethodGetOpacity = "getOpacity";
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
const char * kMethodGetSize = "getSize";
const char * kMethodGetLoc = "getLoc";
}

BONES_API(BonesCore *) BonesStartUp(const BonesConfig & config)
{
    using namespace bones;
    Core::Config cc;
    cc.log_file = config.log_file;
    cc.log_level = static_cast<bones::LogLevel>(config.log_level);

    bool bret = Core::StartUp(cc);
    if (bret)
        bret = LuaContext::StartUp();
    if (bret)
        core = new ScriptParser;

    return core;
}

BONES_API(void) BonesShutDown()
{
    using namespace bones;
    if (core)
        delete core;
    core = nullptr;
    LuaContext::ShutDown();
    Core::ShutDown();
}

BONES_API(void) BonesUpdate()
{
    using namespace bones;
    Core::Update();
}

BONES_API(BonesCore *) BonesGetCore()
{
    return bones::GetCoreInstance();
}




//BONES_API_EXPORT(bones::Pixmap *)BonesDecodePixmap(const void * data, int len)
//{
//    using namespace bones;
//    Pixmap pm;
//    Pixmap * ret = nullptr;
//    if (data && len)
//        pm.decode(data, len);
//    if (pm.isValid())
//        ret = new Pixmap(pm);
//
//
//    return ret;
//}
//
//BONES_API_EXPORT(void)BonesFreePixmap(bones::Pixmap * pm)
//{
//    using namespace bones;
//    if (pm)
//        delete pm;
//}
//
//BONES_API_EXPORT(bones::Ref *) BonesGetCObjectByID(const char * id)
//{
//    using namespace bones;
//    return ScriptParser::Get()->getRefByID(id);
//}
//
//
//BONES_API_EXPORT(void) BonesRegScriptCallback(bones::Ref * co, 
//                                              const char * event_name, 
//                                              bones::ScriptCallBack cb, 
//                                              void * userdata)
//{
//    using namespace bones;
//    ScriptParser::Get()->regScriptCallback(co, event_name, cb, userdata);
//}
//
//BONES_API_EXPORT(void) BonesUnregScriptCallback(bones::Ref * co,
//                                                const char * event_name)
//{
//    using namespace bones;
//    ScriptParser::Get()->unregScriptCallback(co, event_name);
//}
//
//BONES_API_EXPORT(void)BonesApplyCSS(bones::Ref * co, const char * css)
//{
//    using namespace bones;
//    Core::GetCSSManager()->applyCSS(co, css);
//}
//
//BONES_API_EXPORT(void)BonesApplyClass(bones::Ref * co, const char * class_name)
//{
//    using namespace bones;
//    Core::GetCSSManager()->applyClass(co, class_name);
//}

