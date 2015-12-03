#ifndef BONES_CORE_MOUSE_CONTROLLER_H_
#define BONES_CORE_MOUSE_CONTROLLER_H_


#include "event.h"

namespace bones
{

class View;
class Root;
class MouseEvent;

class MouseController
{
public:
    MouseController(Root * root);

    void handleEvent(MouseEvent & e);

    void handleWheel(WheelEvent & e);

    View * capture() const;

    View * over() const;

    void removed(View * n);

    void shiftIfNecessary();
private:
    View * getTargetByPos(const Point & pt, bool ignore_capture);

    void shiftCapture(View * n);

    void shiftOver(View * n);

    void clearIfNecessary();
private:
    Root * root_;
    RefPtr<View> over_;
    RefPtr<View> capture_;
    Point last_mouse_point_;

    FRIEND_TEST(MouseControllerUnitTest, CheckConstructor);
    FRIEND_TEST(MouseControllerUnitTest, Shift);
};


}


#endif