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
extern const char * kMethodGetID;
extern const char * kMethodGetRoot;
extern const char * kMethodGetOpacity;
extern const char * kMethodSetOpacity;
extern const char * kMethodSetFocusable;
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
extern const char * kMethodGetParent;
extern const char * kMethodGetSize;
extern const char * kMethodGetLoc;
extern const char * kMethodSetSize;
extern const char * kMethodSetLoc;

extern const char * kMethodSetColor;

extern const char * kStrCapturing;
extern const char * kStrTarget;
extern const char * kStrBubbling;


//LuaAnimation跟LuaObject不同 要么从C++中创建要么从LUA中创建
//从C++中创建的 是没有lua回调
//从LUA中创建的 是没有C++回调
enum AnimationType
{
    kANI_NATIVE,
    kANI_SCRIPT,
};


#define BONES_CLASS_CALLBACK_0(__selector__, __target__, ...) std::bind(__selector__, __target__, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_1(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_2(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_3(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define BONES_CLASS_CALLBACK_4(__selector__, __target__, ...) std::bind(__selector__, __target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)

#define BONES_CALLBACK_0(__selector__, ...) std::bind(__selector__, ##__VA_ARGS__)
#define BONES_CALLBACK_1(__selector__, ...) std::bind(__selector__, std::placeholders::_1, ##__VA_ARGS__)
#define BONES_CALLBACK_2(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define BONES_CALLBACK_3(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)
#define BONES_CALLBACK_4(__selector__, ...) std::bind(__selector__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, ##__VA_ARGS__)


extern const char * ToEventPhaseStr(BonesEvent::Phase phase);

class EventStack
{
public:
    EventStack();

    ~EventStack();

    int getCount() const;
private:
    static int event_stack_count;
};

}

#endif