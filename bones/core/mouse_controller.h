#ifndef BONES_CORE_MOUSE_CONTROLLER_H_
#define BONES_CORE_MOUSE_CONTROLLER_H_


#include "event.h"

namespace bones
{

class View;
class RootView;
class MouseEvent;

class MouseController
{
public:
    MouseController(RootView * root);

    void handleEvent(MouseEvent & e);

    View * capture() const;

    View * over() const;

    void removed(View * n);

    void shiftIfNecessary();
private:
    View * getTargetByPos(const Point & pt);

    void shiftCapture(View * n);

    void shiftOver(View * n);
private:
    RootView * root_;
    RefPtr<View> over_;
    RefPtr<View> capture_;
    Point last_mouse_point_;
};


}


#endif