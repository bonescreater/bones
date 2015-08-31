#ifndef BONES_CORE_AREA_H_
#define BONES_CORE_AREA_H_

#include "view.h"
#include "event.h"

namespace bones
{

class Area : public View
{
public:
    class Delegate
    {
    public:
        virtual void onMouseEnter(Area * sender, MouseEvent & e) = 0;

        virtual void onMouseMove(Area * sender, MouseEvent & e) = 0;

        virtual void onMouseDown(Area * sender, MouseEvent & e) = 0;

        virtual void onMouseUp(Area * sender, MouseEvent & e) = 0;

        virtual void onMouseClick(Area * sender, MouseEvent & e) = 0;

        virtual void onMouseDClick(Area * sender, MouseEvent & e) = 0;

        virtual void onMouseLeave(Area * sender, MouseEvent & e) = 0;

        virtual void onKeyDown(Area * sender, KeyEvent & e) = 0;

        virtual void onKeyUp(Area * sender, KeyEvent & e) = 0;

        virtual void onKeyPress(Area * sender, KeyEvent & e) = 0;

        virtual void onFocusOut(Area * sender, FocusEvent & e) = 0;

        virtual void onFocusIn(Area * sender, FocusEvent & e) = 0;

        virtual void onBlur(Area * sender, FocusEvent & e) = 0;

        virtual void onFocus(Area * sender, FocusEvent & e) = 0;

        virtual void onWheel(Area * sender, WheelEvent & e) = 0;

        virtual void onSizeChanged(Area * sender, const Size & size) = 0;

        virtual void onPositionChanged(Area * sender, const Point & loc) = 0;

        virtual bool onHitTest(Area * sender, const Point & loc) = 0;
    };

    Area();

    void setDelegate(Delegate * delegate);

    const char * getClassName() const override;
protected:
    void onMouseEnter(MouseEvent & e) override;

    void onMouseMove(MouseEvent & e) override;

    void onMouseDown(MouseEvent & e) override;

    void onMouseUp(MouseEvent & e) override;

    void onMouseClick(MouseEvent & e) override;

    void onMouseDClick(MouseEvent & e) override;

    void onMouseLeave(MouseEvent & e) override;

    void onKeyDown(KeyEvent & e) override;

    void onKeyUp(KeyEvent & e) override;

    void onKeyPress(KeyEvent & e) override;

    void onFocusOut(FocusEvent & e) override;

    void onFocusIn(FocusEvent & e) override;

    void onBlur(FocusEvent & e) override;

    void onFocus(FocusEvent & e) override;

    void onWheel(WheelEvent & e) override;

    void onSizeChanged() override;

    void onPositionChanged()override;

    bool onHitTest(const Point & loc) override;
private:
    Delegate * delegate_;
};


}

#endif