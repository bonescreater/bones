#ifndef BONES_CORE_ACCELERATOR_H_
#define BONES_CORE_ACCELERATOR_H_

#include "keyboard_codes.h"
#include "event.h"

namespace bones
{

class Accelerator
{
public:
    static Accelerator make(const KeyEvent & ke);

    Accelerator();

    Accelerator(EventType type, KeyboardCode keycode, int modifiers);

    Accelerator(const Accelerator & accelerator);

    Accelerator& operator=(const Accelerator& accelerator);

    bool operator <(const Accelerator& rhs) const;

    bool operator ==(const Accelerator& rhs) const;

    bool operator !=(const Accelerator& rhs) const;

    EventType type() const;

    int modifiers() const;

    bool IsShiftDown() const;

    bool IsCtrlDown() const;

    bool IsAltDown() const;

    bool IsCmdDown() const;
protected:
    KeyboardCode key_code_;
    EventType type_;
    int modifiers_;
};

class AcceleratorTarget
{
public:
    virtual bool process(const Accelerator & accelerator) = 0;

    virtual bool canProcess() const = 0;
};


}

#endif