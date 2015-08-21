#ifndef BONES_BONES_H_
#define BONES_BONES_H_

#include <Windows.h>

#ifdef __cplusplus
#define BONES_EXTERN_C extern "C"
#else
#define BONES_EXTERN_C
#endif

#ifdef BONES_DLL
    #define BONES_DECLSPEC_EXPORT __declspec(dllexport)
    #define BONES_API(type) BONES_EXTERN_C BONES_DECLSPEC_EXPORT type __cdecl
#elif defined BONES_LIB
    #define BONES_API(type)
#else
    #define BONES_DECLSPEC_IMPORT __declspec(dllimport)
    #define BONES_API(type) BONES_EXTERN_C BONES_DECLSPEC_IMPORT type __cdecl
#endif

typedef float Scalar;

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

class BonesCore;
BONES_API(BonesCore *) BonesStartUp(const BonesConfig & config);

BONES_API(void) BonesShutDown();

BONES_API(void) BonesUpdate();

BONES_API(BonesCore *) BonesGetCore();

class BonesObject;
class BonesXMLListener
{
public:
    virtual bool onLoad(BonesCore *) = 0;
};

class BonesPixmap
{
    ;
};

struct BonesScriptArg
{
    enum Type
    {
        kString,
        kNumber,
        kBoolean,
        kUserData,
    };

    Type type;
    union
    {
        const char * str;
        double number;
        bool boolean;
        void * ud;
    };
};

class BonesScriptListener
{
public:
    virtual int onEvent(BonesObject * ob, BonesScriptArg * arg, size_t arg_count) = 0;
};

class BonesObject
{
public:
    virtual void retain() = 0;

    virtual void release() = 0;

    virtual const char * getClassName() = 0;

    virtual void * cast() = 0;

    virtual void listen(const char * name, BonesScriptListener * listener) = 0;

    virtual void push(BonesScriptArg * arg) = 0;

    virtual float getOpacity() const = 0;

    virtual void getLoc(Scalar & x, Scalar & y) = 0;

    virtual void getSize(Scalar & w, Scalar & h) = 0;

    virtual bool contains(Scalar x, Scalar y) = 0;

    virtual BonesObject * getChildAt(size_t index) = 0;

    virtual void applyCSS(const char * css) = 0;

    virtual void applyClass(const char * name) = 0;


};

class BonesRoot : public BonesObject
{
public:
    virtual HDC dc() const = 0;
};

class BonesShape : public BonesObject
{

};

class BonesImage : public BonesObject
{

};

class BonesText : public BonesObject
{

};

class BonesRichEdit : public BonesObject
{
    ;
};

class BonesArea : public BonesObject
{

};

class BonesCore
{
public:
    virtual bool loadXMLString(const char * data, BonesXMLListener * listener) = 0;

    virtual void cleanXML() = 0;

    virtual BonesPixmap * createPixmap(const void * data, int len) = 0;

    virtual void destroyPixmap(BonesPixmap *) = 0;

    virtual BonesObject * getObject(const char * id) = 0;

    virtual BonesObject * getObject(BonesObject * ob, const char * id) = 0;
};



#endif