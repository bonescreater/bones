#include "accelerator.h"

namespace bones
{

Accelerator::Accelerator()
:key_code_(kVKEY_UNKNOWN), type_(kET_KEY_DOWN), modifiers_(0)
{
    ;
}

Accelerator::Accelerator(EventType type, KeyboardCode keycode, int modifiers)
: key_code_(keycode), type_(type), modifiers_(modifiers)
{
    ;
}

Accelerator::Accelerator(const Accelerator & accelerator)
{
    key_code_ = accelerator.key_code_;
    modifiers_ = accelerator.modifiers_;
    type_ = accelerator.type_;
}

Accelerator & Accelerator::operator=(const Accelerator & accelerator)
{
    if (this != &accelerator)
    {
        key_code_ = accelerator.key_code_;
        modifiers_ = accelerator.modifiers_;
        type_ = accelerator.type_;
    }
    return *this;
}

bool Accelerator::operator <(const Accelerator& rhs) const
{
    if (key_code_ != rhs.key_code_)
        return key_code_ < rhs.key_code_;
    if (type_ != rhs.type_)
        return type_ < rhs.type_;
    return modifiers_ < rhs.modifiers_;
}

bool Accelerator::operator ==(const Accelerator& rhs) const
{
    return ((key_code_ == rhs.key_code_) && (type_ == rhs.type_) &&
        (modifiers_ == rhs.modifiers_));
}

bool Accelerator::operator !=(const Accelerator& rhs) const
{
    return !(*this == rhs);
}


EventType Accelerator::type() const
{
    return type_;
}

int Accelerator::modifiers() const
{
    return modifiers_;
}

bool Accelerator::IsShiftDown() const
{
    return !!(modifiers_ & kEF_SHIFT_DOWN);
}

bool Accelerator::IsCtrlDown() const
{
    return !!(modifiers_ & kEF_CONTROL_DOWN);
}

bool Accelerator::IsAltDown() const
{
    return !!(modifiers_ & kEF_ALT_DOWN);
}

bool Accelerator::IsCmdDown() const
{
    return !!(modifiers_ & kEF_COMMAND_DOWN);
}

Accelerator Accelerator::make(const KeyEvent & ke)
{
    int modifiers = kEF_NONE;
    if (ke.isShiftDown())
        modifiers |= kEF_SHIFT_DOWN;
    if (ke.isControlDown())
        modifiers |= kEF_CONTROL_DOWN;
    if (ke.isAltDown())
        modifiers |= kEF_ALT_DOWN;
    return Accelerator(ke.type(), ke.key(), modifiers);
}

}