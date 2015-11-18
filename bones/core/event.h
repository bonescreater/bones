#ifndef BONES_CORE_EVENT_H_
#define BONES_CORE_EVENT_H_

#include "ref.h"
#include "keyboard_codes.h"
#include "point.h"
#include "event_dispatcher.h"

namespace bones
{

class View;
enum EventType
{
    kET_MOUSE_ENTER = 0,
    kET_MOUSE_MOVE,
    kET_MOUSE_DOWN,
    kET_MOUSE_UP,
    kET_MOUSE_CLICK,
    kET_MOUSE_DCLICK,
    kET_MOUSE_LEAVE,
    kET_MOUSE_WHEEL,

    kET_KEY_DOWN,
    kET_CHAR,
    kET_KEY_UP,

    kET_COMPOSITION_START,
    kET_COMPOSITION_UPDATE,
    kET_COMPOSITION_END,

    kET_FOCUS_OUT,
    kET_FOCUS_IN,
    kET_BLUR,
    kET_FOCUS,

    

    kET_CUSTOM,
    kET_COUNT,
};
//like cef
enum EventFlags
{
    kEF_NONE = 0,       // Used to denote no flags explicitly
    kEF_CAPS_LOCK_ON = 1 << 0,
    kEF_SHIFT_DOWN = 1 << 1,
    kEF_CONTROL_DOWN = 1 << 2,
    kEF_ALT_DOWN = 1 << 3,
    kEF_LEFT_MOUSE_DOWN = 1 << 4,
    kEF_MIDDLE_MOUSE_DOWN = 1 << 5,
    kEF_RIGHT_MOUSE_DOWN = 1 << 6,
    kEF_COMMAND_DOWN = 1 << 7,  // GUI Key (e.g. Command on OS X keyboards,
    // Search on Chromebook keyboards,
    // Windows on MS-oriented keyboards)
    kEF_NUM_LOCK_ON = 1 << 8,
    kEF_IS_KEY_PAD = 1 << 9,
    kEF_IS_LEFT = 1 << 10,
    kEF_IS_RIGHT = 1 << 11,
};

class Event
{
    friend class EventDispatcher;
public:
    typedef int Type;

    enum Phase
    {
        kCapturing,
        kTarget,
        kBubbling,

        kPhaseCount,//area delegates
    };

    void stopPropagation();

    void preventDefault();

    bool canceled() const;

    View * target() const;

    EventType type() const;

    Phase phase() const;

    void attach(const EventDispatcher::Path * path);

    const EventDispatcher::Path * getPath() const;
protected:
    Event();
protected:
    EventType type_;
    RefPtr<View> target_;
    Phase phase_;
    bool bubbles_;
    bool cancelable_;
    bool canceled_;
    bool propagation_;
    const EventDispatcher::Path * path_;
};

class UIEvent : public Event
{
public:
    bool isCapsLockOn() const;

    bool isShiftDown() const;

    bool isControlDown() const;

    bool isAltDown() const;

    bool isLeftMouseDown() const;

    bool isMiddleMouseDown() const;

    bool isRightMouseDown() const;

    bool isCommandDown() const;

    bool isNumLockOn() const;

    bool isKeyPad() const;

    bool isLeft() const;

    bool isRight() const;

    int getFlags() const;
protected:
    int flags_;
};

enum MouseButton
{
    kMB_NONE = kEF_NONE,
    kMB_LEFT = kEF_LEFT_MOUSE_DOWN,
    kMB_MIDDLE = kEF_MIDDLE_MOUSE_DOWN,
    kMB_RIGHT = kEF_RIGHT_MOUSE_DOWN,
};

class MouseEvent : public UIEvent
{
public:
    static MouseEvent * From(Event & e);
public:
    MouseEvent(EventType type, MouseButton button, View * target,
               const Point & location,
               const Point & root_location,
               int flags);

    bool isLeftMouse() const;   

    bool isMiddleMouse() const;   

    bool isRightMouse() const;

    const Point & getLoc() const;
    
    const Point & getRootLoc() const;

    MouseButton button() const;
private:
    Point location_;
    Point root_location_;
    MouseButton button_;
};

// equal lParam
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

class KeyEvent : public UIEvent
{
public:
    static KeyEvent * From(Event & e);
public:
    KeyEvent(EventType type, View * target, KeyboardCode value, KeyState state, bool system, int flags);

    KeyboardCode key() const;

    wchar_t ch() const;

    KeyState state() const;

    bool system() const;
private:
    union
    {
        KeyboardCode key_code_;
        wchar_t ch_;
    }; 
    KeyState state_;
    bool system_;
};

class FocusEvent : public UIEvent
{
public:
    static FocusEvent * From(Event & e);
public:
    FocusEvent(EventType type, View * target, bool tab);

    bool isTabTraversal() const;
private:
    bool tab_traversal_;
};

class WheelEvent : public MouseEvent
{
public:
    static WheelEvent * From(Event & e);

    WheelEvent(EventType type, View * target, int dx, int dy, 
               const Point & location,
               const Point & root_location, int flags);

    int dx() const;

    int dy() const;
private:
    int dx_;
    int dy_;
};

class CompositionEvent : public Event
{
public:
    static CompositionEvent * From(Event & e);

    CompositionEvent(EventType type, View * target, 
        long index, unsigned int dbcs, 
        const wchar_t * str,
        long cursor);

    long index() const;

    unsigned int dbcs() const;

    const wchar_t * str() const;

    long cursor() const;
private:
    long index_;
    unsigned int dbcs_;
    const wchar_t * str_;
    long cursor_;
};

}
#endif