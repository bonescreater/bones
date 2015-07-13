#ifndef BONES_CORE_AREA_H_
#define BONES_CORE_AREA_H_

#include "view.h"
#include "event.h"
#include <vector>
#include <functional>

namespace bones
{

class Area : public View
{
public:
    typedef std::function <void(Area & sender, Event & e)> CFEvent;
    typedef std::function <void(Area & sender)> CFNotifyOnSizeChanged;

    Area();

    const char * getClassName() const override;

    void bind(EventType type, Event::Phase phase, const CFEvent & cf);

    void bind(const CFNotifyOnSizeChanged & cf);
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

    void onFocusOut(FocusEvent & e) override;

    void onFocusIn(FocusEvent & e) override;

    void onBlur(FocusEvent & e) override;

    void onFocus(FocusEvent & e) override;

    void onWheel(WheelEvent & e) override;

    void onSizeChanged() override;
private:
    void pushEvent(Event & e);
private:
    CFEvent delegates_[Event::kPhaseCount][kET_COUNT];
    CFNotifyOnSizeChanged on_size_changed_;
};


}

#endif