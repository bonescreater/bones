#ifndef BONES_INTERNAL_H_
#define BONES_INTERNAL_H_

#include "bones.h"


namespace bones
{
class ScriptParser;

extern ScriptParser * GetCoreInstance();

extern const char * kNotifyOrder;
extern const char * kEventOrder;

extern const char * kMethodOnCreate;
extern const char * kMethodOnDestroy;

extern const char * kMethodAnimateRun;
extern const char * kMethodAnimateStart;
extern const char * kMethodAnimateStop;
extern const char * kMethodAnimatePause;
extern const char * kMethodAnimateResume;

extern const char * kMethodOnMouseDown;
extern const char * kMethodOnMouseUp;
extern const char * kMethodOnMouseMove;
extern const char * kMethodOnMouseLeave;
extern const char * kMethodOnMouseEnter;
extern const char * kMethodOnMouseClick;
extern const char * kMethodOnMouseDClick;
extern const char * kMethodOnKeyDown;
extern const char * kMethodOnChar;
extern const char * kMethodOnKeyUp;
extern const char * kMethodOnFocusIn;
extern const char * kMethodOnFocusOut;
extern const char * kMethodOnFocus;
extern const char * kMethodOnBlur;
extern const char * kMethodOnWheel;

extern const char * kMethodOnSizeChanged;
extern const char * kMethodOnPositionChanged;
extern const char * kMethodOnHitTest;

extern const char * kMethodIndex;
extern const char * kMethodGC;
extern const char * kMethodGetOpacity;
extern const char * kMethodGetCObject;
extern const char * kMethodAnimate;
extern const char * kMethodStop;
extern const char * kMethodPause;
extern const char * kMethodResume;
extern const char * kMethodStopAll;
extern const char * kMethodContains;
extern const char * kMethodApplyCSS;
extern const char * kMethodApplyClass;
extern const char * kMethodGetChildAt;
extern const char * kMethodGetSize;
extern const char * kMethodGetLoc;
//LuaAnimation跟LuaObject不同 要么从C++中创建要么从LUA中创建
//从C++中创建的 是没有lua回调
//从LUA中创建的 是没有C++回调
enum AnimationType
{
    kANI_NATIVE,
    kANI_SCRIPT,
};


}

#endif