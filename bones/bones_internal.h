#ifndef BONES_INTERNAL_H_
#define BONES_INTERNAL_H_

#include "bones.h"


namespace bones
{
class ScriptParser;

extern ScriptParser * GetCoreInstance();

extern const char * kNotifyOrder;

extern const char * kMethodAnimateRun;
extern const char * kMethodAnimateStart;
extern const char * kMethodAnimateStop;
extern const char * kMethodAnimatePause;
extern const char * kMethodAnimateResume;

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