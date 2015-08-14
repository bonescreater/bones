#ifndef BONES_CORE_WIDGET_H_
#define BONES_CORE_WIDGET_H_

#include "core.h"
#include "ref.h"
#include "css_types.h"

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

    BONES_CSS_TABLE_DECLARE()

    void setLeft(const CSSParams & params);

    void setTop(const CSSParams & params);

    void setWidth(const CSSParams & params);

    void setHeight(const CSSParams & params);

    void setContent(const CSSParams & params);
protected:
    HWND hwnd_;
};


}

#endif