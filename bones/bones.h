#ifndef BONES_BONES_H_
#define BONES_BONES_H_


#include "core/core.h"
#include "lua.hpp"


#ifdef __cplusplus
#define BONES_EXTERN_C extern "C"
#else
#define BONES_EXTERN_C
#endif

#define BONES_DECLSPEC_EXPORT __declspec(dllexport)

#define BONES_API_EXPORT(type) BONES_EXTERN_C BONES_DECLSPEC_EXPORT type __cdecl

namespace bones
{
class Ref;
class Pixmap;

extern const char * kStrCapture;
extern const char * kStrTarget;
extern const char * kStrBubbling;

typedef union
{
    const char * c;
    float f;
    bool b;
    void * v;
} ScriptArg;

typedef bool(*ScriptCallBack)(Ref * ob, ScriptArg * arg, size_t arg_count, void * userdata);

}





#endif