#ifndef BONES_BONES_API_H_
#define BONES_BONES_API_H_

#ifdef __cplusplus
    #define BONES_EXTERN_C extern "C"
#else
    #define BONES_EXTERN_C
#endif

#define BONES_DECLSPEC_IMPORT __declspec(dllimport)

#define BONES_API_IMPORT(type) BONES_EXTERN_C BONES_DECLSPEC_IMPORT type __cdecl

enum BonesLogLevel
{
    kBONES_LOG_LEVEL_NONE = 0,
    kBONES_LOG_LEVEL_ERROR = 1,// only error
    kBONES_LOG_LEVEL_VERBOSE = 2,// everything
};

struct BonesConfig
{
    const wchar_t * log_file;
    BonesLogLevel log_level;
};

typedef void * BonesCObject;
struct Pixmap;

typedef union
{
    const char * c;
    float f;
    bool b;
    void * v;
} BonesArg;

typedef bool(*BonesScriptCallBack)(BonesCObject co, BonesArg * arg, size_t arg_count, void *userdata);

BONES_API_IMPORT(bool) BonesStartUp(const BonesConfig & config);
//
BONES_API_IMPORT(void) BonesShutDown();

BONES_API_IMPORT(void) BonesUpdate();

BONES_API_IMPORT(const wchar_t *) BonesGetVersion();

BONES_API_IMPORT(bool) BonesLoadXMLString(const char * data);

BONES_API_IMPORT(void) BonesCleanXML();

//resource api
BONES_API_IMPORT(Pixmap *)BonesDecodePixmap(const void * data, int len);

BONES_API_IMPORT(void)BonesFreePixmap(Pixmap * pm);

//object
BONES_API_IMPORT(BonesCObject) BonesGetCObjectByID(const char * id);
//script object
BONES_API_IMPORT(void) BonesRegScriptCallback(BonesCObject co, const char * event_name, BonesScriptCallBack cb, void *userdata);

BONES_API_IMPORT(void) BonesUnregScriptCallback(BonesCObject co, const char * event_name);
//c object
BONES_API_IMPORT(void)BonesApplyCSS(BonesCObject co, const char * css);

BONES_API_IMPORT(void)BonesApplyClass(BonesCObject co, const char * class_name, const char * mod);




#endif
