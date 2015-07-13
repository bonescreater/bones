#ifndef BONES_CORE_WIDGET_H_
#define BONES_CORE_WIDGET_H_

#include "core.h"
#include "ref.h"

namespace bones
{

class Size;
class Point;
class Rect;

class Widget : public Ref
{
public:
    Widget();

    virtual ~Widget();

    bool isValid() const;

    bool show(bool visible);

    bool restore();

    bool maximize();

    bool minimize();

    bool close();

    bool setLeft(Scalar left);

    bool setTop(Scalar top);

    bool setLoc(const Point & pt);

    bool setWidth(Scalar width);

    bool setHeight(Scalar height);

    bool setSize(const Size & size);

    bool getWindowRect(Rect & rect);

    bool getClientRect(Rect & rect);

    bool setWindowText(const wchar_t * text);

    void attach(HWND hwnd);

    void detach();

    HWND hwnd() const;

    const char * getClassName() const override;
protected:
    HWND hwnd_;
};


}

#endif