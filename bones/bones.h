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
typedef HBITMAP BonesCaret;

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

    virtual void onUnload(BonesCore *) = 0;
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

class BonesEvent
{
public:
    enum Phase
    {
        kCapture,
        kTarget,
        kBubbling,

        kPhaseCount,
    };
public:
    virtual void stopPropagation() = 0;

    virtual void preventDefault() = 0;

    virtual bool canceled() = 0;

    virtual BonesObject * target() = 0;

    virtual Phase phase() = 0;
};

class BonesEventFlag
{
public:
    virtual bool isShiftDown() const = 0;

    virtual bool isControlDown() const = 0;

    virtual bool isCapsLockDown() const = 0;

    virtual bool isAltDown() const = 0;

    virtual bool isAltGrDown() const = 0;

    virtual bool isCommandDown() const = 0;

    virtual bool isLeftMouseDown() const = 0;

    virtual bool isMiddleMouseDown() const = 0;

    virtual bool isRightMouseDown() const = 0;
};

class BonesMouseEvent : public BonesEvent, public BonesEventFlag
{
public:
    enum Type
    {
        kMouseEnter,
        kMouseMove,
        kMouseDown,
        kMouseUp,
        kMouseClick,
        kMouseDClick,
        kMouseLeave,

        kTypeCount,
    };

    virtual Type type() const = 0;

    virtual bool isLeftMouse() const = 0; 

    virtual bool isMiddleMouse() const = 0;

    virtual bool isRightMouse() const = 0;

    virtual BonesPoint getLoc() const = 0;

    virtual BonesPoint getRootLoc() const = 0;
};

class BonesKeyEvent : public BonesEvent, public BonesEventFlag
{
public:
    enum Type
    {
        kKeyDown,
        kKeyPress,
        kKeyUp,

        kTypeCount,
    };

    struct KeyState
    {
        union
        {
            struct
            {
                uint32_t repeat_count : 16;
                uint32_t scan_code : 8;
                uint32_t extended_key : 1;
                uint32_t reserved : 4;
                uint32_t context_code : 1;
                uint32_t previous_state : 1;
                uint32_t transition_state : 1;
            };
            uint32_t state;
        };
    };

    virtual Type type() const = 0;

    virtual wchar_t ch() const = 0;

    virtual KeyState state() const = 0;
};

class BonesFocusEvent : public BonesEvent
{
public:
    enum Type
    {
        kFocusIn,
        kFocusOut,
        kBlur,
        kFocus,

        kTypeCount,
    };

    virtual Type type() const = 0;

    virtual bool isTabTraversal() const = 0;
};

class BonesWheelEvent : public BonesEvent, public BonesEventFlag
{
public:
    virtual int dx() const = 0;

    virtual int dy() const = 0;
};

class BonesBase
{
public:
    virtual void retain() = 0;

    virtual void release() = 0;
};

class BonesAnimation : public BonesBase
{
public:
    enum Action
    {
        kStart,
        kStop,
        kPause,
        kResume,
    };
    class RunListener
    {
    public:
        virtual void onEvent(BonesAnimation * ani, BonesObject * ob, float progress) = 0;
    };

    class ActionListener
    {
    public:
        virtual void onEvent(BonesAnimation * ani, BonesObject * ob, Action action) = 0;
    };
};

class BonesObject : public BonesBase
{
public:
    //每个Object都支持onPrepare事件
    template<class T>
    class NotifyBase
    {
    public:
        virtual void onPrepare(T * sender, bool & stop) = 0;
    };
public:
    virtual const char * getClassName() = 0;

    virtual void * cast() = 0;

    virtual void listen(const char * name, BonesScriptListener * listener) = 0;

    virtual void push(BonesScriptArg * arg) = 0;

    virtual float getOpacity() const = 0;

    virtual BonesPoint getLoc() const = 0;

    virtual BonesSize getSize() const = 0;

    virtual void setVisible(bool visible) = 0;

    virtual void setFocusable(bool focusable) = 0;

    virtual bool contains(BonesScalar x, BonesScalar y) = 0;

    virtual BonesObject * getChildAt(size_t index) = 0;

    virtual void applyCSS(const char * css) = 0;

    virtual void applyClass(const char * name) = 0;

    virtual BonesAnimation * animate(
        uint64_t interval, uint64_t due, 
        BonesAnimation::RunListener * run,
        BonesAnimation::ActionListener * stop,
        BonesAnimation::ActionListener * start,
        BonesAnimation::ActionListener * pause,
        BonesAnimation::ActionListener * resume) = 0;

    virtual void stopAnimate(BonesAnimation * ani, bool toend) = 0;

    virtual void pauseAnimate(BonesAnimation * ani) = 0;

    virtual void resumeAnimate(BonesAnimation * ani) = 0;

    virtual void stopAllAnimate(bool toend) = 0;
};

class BonesRoot : public BonesObject
{
public:
    class NotifyListener : public NotifyBase<BonesRoot>
    {
    public:
        virtual void requestFocus(BonesRoot * sender, bool & stop) = 0;

        virtual void invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop) = 0;

        virtual void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) = 0;

        virtual void showCaret(BonesRoot * sender, bool fshow, bool & stop) = 0;

        virtual void createCaret(BonesRoot * sender, BonesCaret hbmp, const BonesSize & size, bool & stop) = 0;

        virtual void changeCaretPos(BonesRoot * sender, const BonesPoint & pt, bool & stop) = 0;

        virtual void onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop) = 0;

        virtual void onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop) = 0;
    };

    virtual void setListener(NotifyListener * listener) = 0;

    virtual bool isDirty() const = 0;

    virtual BonesRect getDirtyRect() const = 0;

    virtual void draw() = 0;

    virtual HBITMAP getBackBuffer() const = 0;

    virtual void getBackBuffer(const void * & data, size_t & pitch) const = 0;

    virtual void handleMouse(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleKey(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleFocus(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleComposition(UINT msg, WPARAM wparam, LPARAM lparam) = 0;

    virtual void handleWheel(UINT msg, WPARAM wparam, LPARAM lparam) = 0;
};

class BonesShape : public BonesObject
{
public:
    class NotifyListener : public NotifyBase<BonesShape>
    {
        ;
    };

    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesImage : public BonesObject
{
public:
    class NotifyListener : public NotifyBase<BonesImage>
    {
        ;
    };

    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesText : public BonesObject
{
public:
    class NotifyListener : public NotifyBase<BonesText>
    {
        ;
    };

    virtual void setListener(NotifyListener * lis) = 0;
};

class BonesRichEdit : public BonesObject
{
public:
    class NotifyListener : public NotifyBase<BonesRichEdit>
    {
    public:
        virtual BOOL screenToClient(BonesRichEdit * sender, LPPOINT lppt, bool & stop) = 0;

        virtual BOOL clientToScreen(BonesRichEdit * sender, LPPOINT lppt, bool & stop) = 0;

        virtual HIMC immGetContext(BonesRichEdit * sender, bool & stop) = 0;

        virtual void immReleaseContext(BonesRichEdit * sender, HIMC himc, bool & stop) = 0;

        virtual HRESULT txNotify(BonesRichEdit * sender, DWORD iNotify, void  *pv, bool & stop) = 0;

        virtual void onReturn(BonesRichEdit * sender, bool & stop) = 0;
    };

    virtual void setListener(NotifyListener * listener) = 0;
};

class BonesArea : public BonesObject
{
public:
    class MouseListener
    {
    public:
        virtual void onMouseEnter(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseMove(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseDown(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseUp(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseClick(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseDClick(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;

        virtual void onMouseLeave(BonesObject * sender, BonesMouseEvent & e, bool & stop) = 0;
    };

    class KeyListener
    {
    public:
        virtual void onKeyDown(BonesObject * sender, BonesKeyEvent & e, bool & stop) = 0;

        virtual void onKeyUp(BonesObject * sender, BonesKeyEvent & e, bool & stop) = 0;

        virtual void onKeyPress(BonesObject * sender, BonesKeyEvent & e, bool & stop) = 0;
    };

    class FocusListener
    {
    public:
        virtual void onFocusOut(BonesObject * sender, BonesFocusEvent & e, bool & stop) = 0;

        virtual void onFocusIn(BonesObject * sender, BonesFocusEvent & e, bool & stop) = 0;

        virtual void onBlur(BonesObject * sender, BonesFocusEvent & e, bool & stop) = 0;

        virtual void onFocus(BonesObject * sender, BonesFocusEvent & e, bool & stop) = 0;
    };

    class WheelListener
    {
    public:
        virtual void onWheel(BonesObject * sender, BonesWheelEvent & e, bool & stop) = 0;
    };

    class NotifyListener : public NotifyBase<BonesArea>
    {
    public:
        virtual void onSizeChanged(BonesArea * sender, const BonesSize & size, bool & stop) = 0;

        virtual void onPositionChanged(BonesArea * sender, const BonesPoint & loc, bool & stop) = 0;

        virtual bool onHitTest(BonesArea * sender, const BonesPoint & loc, bool & stop) = 0;
    };

    virtual void setListener(MouseListener * lis) = 0;

    virtual void setListener(KeyListener * lis) = 0;

    virtual void setListener(FocusListener * lis) = 0;

    virtual void setListener(WheelListener * lis) = 0;

    virtual void setListener(NotifyListener * notify) = 0;
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