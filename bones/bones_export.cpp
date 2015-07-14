#include "bones_export.h"
#include "script_parser.h"
#include "lua_context.h"
#include "core/pixmap.h"

namespace bones
{

const char * kStrCapture = "capture";
const char * kStrTarget = "target";
const char * kStrBubbling = "bubbling";
//const char * kStrETMouseEnter = "et-mouse-enter";
//const char * kStrETMouseMove = "et-mouse-move";
//const char * kStrETMouseDown = "et-mouse-down";
//const char * kStrETMouseUp = "et-mouse-up";
//const char * kStrETMouseClick = "et-mouse-click";
//const char * kStrETMouseDClick = "et-mouse-dclick";
//const char * kStrETMouseLeave = "et-mouse-leave";
//const char * kStrETMouseWHEEL = "et-mouse-wheel";
//const char * kStrETKeyDown = "et-key-down";
//const char * kStrETKeyUp = "et-key-up";
//const char * kStrETFocusOut = "et-focus-out";
//const char * kStrETFocusIn = "et-focus-in";
//const char * kStrETBlur = "et-blur";
//const char * kStrETFocus = "et-focus";
//const char * kStrETCustom = "et-custom";
//const char * kStrETUnknown = "et-unknown";

}

BONES_API_EXPORT(bool) BonesStartUp(const bones::Core::Config & config)
{
    using namespace bones;
    bool bret = Core::StartUp(config);

    if (bret)
        bret = LuaContext::StartUp();

    return bret;
}

BONES_API_EXPORT(void) BonesShutDown()
{
    using namespace bones;
    LuaContext::ShutDown();
    Core::ShutDown();
}

BONES_API_EXPORT(void) BonesUpdate()
{
    using namespace bones;
    Core::Update();
}

BONES_API_EXPORT(const wchar_t *) BonesGetVersion()
{
    static const wchar_t * version =
        L"skia unknown\n"
        L"rapidxml 1.13\n"
        L"lua 5.3.0\n"
        L"bones 0.1 and developing\n";
    return version;
}

BONES_API_EXPORT(bool) BonesLoadXMLString(const char * data)
{
    using namespace bones;
    return ScriptParser::Get()->loadString(data);
}

BONES_API_EXPORT(void) BonesCleanXML()
{
    using namespace bones;
    return ScriptParser::Get()->clean();
}

BONES_API_EXPORT(bones::Pixmap *)BonesDecodePixmap(const void * data, int len)
{
    using namespace bones;
    Pixmap pm;
    Pixmap * ret = nullptr;
    if (data && len)
        pm.decode(data, len);
    if (pm.isValid())
        ret = new Pixmap(pm);


    return ret;
}

BONES_API_EXPORT(void)BonesFreePixmap(bones::Pixmap * pm)
{
    using namespace bones;
    if (pm)
        delete pm;
}

BONES_API_EXPORT(bones::Ref *) BonesGetCObjectByID(const char * id)
{
    using namespace bones;
    return ScriptParser::Get()->getRefByID(id);
}


BONES_API_EXPORT(void) BonesRegScriptCallback(bones::Ref * co, 
                                              const char * event_name, 
                                              bones::ScriptCallBack cb, 
                                              void * userdata)
{
    using namespace bones;
    ScriptParser::Get()->regScriptCallback(co, event_name, cb, userdata);
}

BONES_API_EXPORT(void) BonesUnregScriptCallback(bones::Ref * co,
                                                const char * event_name)
{
    using namespace bones;
    ScriptParser::Get()->unregScriptCallback(co, event_name);
}

BONES_API_EXPORT(void)BonesApplyCSS(bones::Ref * co, const char * css)
{
    using namespace bones;
    ScriptParser::Get()->applyCSS(co, css);
}

BONES_API_EXPORT(void)BonesApplyClass(bones::Ref * co, const char * class_name, const char * mod)
{
    using namespace bones;
    ScriptParser::Get()->applyClass(co, class_name, mod);
}

