#ifndef BONES_BONES_H_
#define BONES_BONES_H_

#include <Windows.h>
#include <stdint.h>

#ifdef __cplusplus
#define BONES_EXTERN_C extern "C"
#else
#define BONES_EXTERN_C
#endif

#ifdef BONES_BUILD_DLL
    #define BONES_DECLSPEC_EXPORT __declspec(dllexport)
    #define BONES_API(type) BONES_EXTERN_C BONES_DECLSPEC_EXPORT type __cdecl
#elif defined BONES_BUILD_LIB
    #define BONES_API(type)
#else
    #define BONES_DECLSPEC_IMPORT __declspec(dllimport)
    #define BONES_API(type) BONES_EXTERN_C BONES_DECLSPEC_IMPORT type __cdecl
#endif

typedef float BonesScalar;
typedef HANDLE BonesCursor;

typedef struct
{
    BonesScalar left;
    BonesScalar top;
    BonesScalar right;
    BonesScalar bottom;
}BonesRect;

typedef struct
{
    BonesScalar width;
    BonesScalar height;
}BonesSize;

typedef struct
{
    BonesScalar x;
    BonesScalar y;
}BonesPoint;

typedef uint32_t BonesColor;

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

class BonesBase
{
public:
    virtual void retain() = 0;

    virtual void release() = 0;
};

class BonesObject : public BonesBase
{
public:
    virtual const char * getClassName() = 0;

    virtual void * cast() = 0;

    virtual void listen(const char * name, BonesScriptListener * listener) = 0;

    virtual void push(BonesScriptArg * arg) = 0;

    virtual float getOpacity() const = 0;

    virtual BonesPoint getLoc() const = 0;

    virtual BonesSize getSize() const = 0;

    virtual bool contains(BonesScalar x, BonesScalar y) = 0;

    virtual BonesObject * getChildAt(size_t index) = 0;

    virtual void applyCSS(const char * css) = 0;

    virtual void applyClass(const char * name) = 0;
};

class BonesRoot : public BonesObject
{
public:
    class Listener
    {
    public:
        virtual void requestFocus(BonesRoot * sender, bool & stop) = 0;

        virtual void invalidRect(BonesRoot * sender, BonesRect & rect, bool & stop) = 0;

        virtual void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) = 0;

        virtual void onSizeChanged(BonesRoot * sender, BonesSize size, bool & stop) = 0;

        virtual void onPositionChanged(BonesRoot * sender, BonesPoint loc, bool & stop) = 0;
    };

    virtual void setListener(Listener * listener) = 0;

    virtual bool isDirty() const = 0;

    virtual BonesRect getDirtyRect() const = 0;

    virtual void draw() = 0;

    virtual HDC dc() const = 0;

    virtual void handleMouse(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleKey(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleFocus(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleComposition(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleWheel(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
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
public:
    class Listener
    {
    public:
        virtual void killTimer(BonesRichEdit * sender, UINT idTimer, bool & stop) = 0;

        virtual BOOL setTimer(BonesRichEdit * sender, UINT idTimer, UINT uTimeout, bool & stop) = 0;

        virtual BOOL showCaret(BonesRichEdit * sender, BOOL fshow, bool & stop) = 0;

        virtual BOOL createCaret(BonesRichEdit * sender, HBITMAP hbmp, INT xWidth, INT yHeight, bool & stop) = 0;

        virtual BOOL setCaretPos(BonesRichEdit * sender, INT x, INT y, bool & stop) = 0;

        virtual BOOL screenToClient(BonesRichEdit * sender, LPPOINT lppt, bool & stop) = 0;

        virtual BOOL clientToScreen(BonesRichEdit * sender, LPPOINT lppt, bool & stop) = 0;

        virtual HIMC immGetContext(BonesRichEdit * sender, bool & stop) = 0;

        virtual void immReleaseContext(BonesRichEdit * sender, HIMC himc, bool & stop) = 0;
    };

    virtual void setListener(Listener * listener) = 0;
};

class BonesArea : public BonesObject
{

};

class BonesPixmap : public BonesBase
{
public:
    struct LockRec
    {
        void * bits;
        size_t pitch;
        BonesRect subset;
    };
public:
    virtual bool alloc(int width, int height) = 0;

    virtual bool decode(const void * data, size_t len) = 0;

    virtual int getWidth() const = 0;

    virtual int getHeight() const = 0;

    virtual bool lock(LockRec & rec) = 0;

    virtual void unlock() = 0;

    virtual void erase(BonesColor color) = 0;

    virtual void extractSubset(BonesPixmap & pm, const BonesRect & subset) = 0;
};

class BonesResManager
{
public:
    virtual void add(const char * key, BonesPixmap & pm) = 0;

    virtual void add(const char * key, BonesCursor cursor) = 0;

    virtual void clean() = 0;
};

class BonesCore
{
public:
    virtual bool loadXMLString(const char * data, BonesXMLListener * listener) = 0;

    virtual void cleanXML() = 0;

    virtual BonesPixmap * create() = 0;

    virtual BonesResManager * getResManager() = 0;

    virtual BonesObject * getObject(const char * id) = 0;

    virtual BonesObject * getObject(BonesObject * ob, const char * id) = 0;
};



#endif