#ifndef BONES_CORE_AREA_H_
#define BONES_CORE_AREA_H_

#include "view.h"
#include "event.h"

namespace bones
{

template <class T>
class Area : public View
{
public:
    class DelegateBase
    {
    public:
        virtual void onMouseEnter(T * sender, MouseEvent & e) = 0;

        virtual void onMouseMove(T * sender, MouseEvent & e) = 0;

        virtual void onMouseDown(T * sender, MouseEvent & e) = 0;

        virtual void onMouseUp(T * sender, MouseEvent & e) = 0;

        virtual void onMouseClick(T * sender, MouseEvent & e) = 0;

        virtual void onMouseDClick(T * sender, MouseEvent & e) = 0;

        virtual void onMouseLeave(T * sender, MouseEvent & e) = 0;

        virtual void onKeyDown(T * sender, KeyEvent & e) = 0;

        virtual void onKeyUp(T * sender, KeyEvent & e) = 0;

        virtual void onChar(T * sender, KeyEvent & e) = 0;

        virtual void onFocusOut(T * sender, FocusEvent & e) = 0;

        virtual void onFocusIn(T * sender, FocusEvent & e) = 0;

        virtual void onBlur(T * sender, FocusEvent & e) = 0;

        virtual void onFocus(T * sender, FocusEvent & e) = 0;

        virtual void onWheel(T * sender, WheelEvent & e) = 0;

        virtual void onSizeChanged(T * sender, const Size & size) = 0;

        virtual void onPositionChanged(T * sender, const Point & loc) = 0;

        virtual bool onHitTest(T * sender, const Point & loc) = 0;
    };
protected:
    virtual DelegateBase * delegate() = 0;

    T * cast()
    {
        return static_cast<T *>(this);
    }
protected:
    void onMouseEnter(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseEnter(cast(), e) : 0;
    }

    void onMouseMove(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseMove(cast(), e) : 0;
    }

    void onMouseDown(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseDown(cast(), e) : 0;
    }

    void onMouseUp(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseUp(cast(), e) : 0;
    }

    void onMouseClick(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseClick(cast(), e) : 0;
    }

    void onMouseDClick(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseDClick(cast(), e) : 0;
    }

    void onMouseLeave(MouseEvent & e) override
    {
        delegate() ? delegate()->onMouseLeave(cast(), e) : 0;
    }

    void onKeyDown(KeyEvent & e) override
    {
        delegate() ? delegate()->onKeyDown(cast(), e) : 0;
    }

    void onKeyUp(KeyEvent & e) override
    {
        delegate() ? delegate()->onKeyUp(cast(), e) : 0;
    }

    void onChar(KeyEvent & e) override
    {
        delegate() ? delegate()->onChar(cast(), e) : 0;
    }

    void onFocusOut(FocusEvent & e) override
    {
        delegate() ? delegate()->onFocusOut(cast(), e) : 0;
    }

    void onFocusIn(FocusEvent & e) override
    {
        delegate() ? delegate()->onFocusIn(cast(), e) : 0;

    }

    void onBlur(FocusEvent & e) override
    {
        delegate() ? delegate()->onBlur(cast(), e) : 0;
    }

    void onFocus(FocusEvent & e) override
    {
        delegate() ? delegate()->onFocus(cast(), e) : 0;
    }

    void onWheel(WheelEvent & e) override
    {
        delegate() ? delegate()->onWheel(cast(), e) : 0;
    }

    void onSizeChanged() override
    {
        delegate() ? delegate()->onSizeChanged(cast(), getSize()) : 0;
    }

    void onPositionChanged() override
    {
        delegate() ? delegate()->onPositionChanged(cast(), getLoc()) : 0;
    }

    bool onHitTest(const Point & loc) override
    {
        return delegate() ? delegate()->onHitTest(cast(), loc) : true;
    }
};


}

#endif