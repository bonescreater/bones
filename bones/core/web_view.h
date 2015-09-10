#ifndef BONES_CORE_WEB_VIEW_H_
#define BONES_CORE_WEB_VIEW_H_

#include "view.h"
#include "pixmap.h"


namespace bones
{

class Browser;

class WebView : public View 
{
public:
    static bool StartUp(const char * locate);

    static void ShutDown();

    static void Update();

    WebView();

    ~WebView();

    bool open();

    void close();

    const char * getClassName() const override;
protected:
    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onSizeChanged() override;

    void onMouseEnter(MouseEvent & e) override;

    void onMouseLeave(MouseEvent & e) override;

    void onMouseMove(MouseEvent & e) override;

    void onMouseDown(MouseEvent & e) override;

    void onMouseUp(MouseEvent & e) override;

    void onWheel(WheelEvent & e) override;

    void onFocus(FocusEvent & e) override;

    void onBlur(FocusEvent & e) override;

    void onKeyDown(KeyEvent & e) override;

    void onKeyUp(KeyEvent & e) override;

    void onChar(KeyEvent & e) override;
private:
    void adjustPixmap();
private:
    Browser * browser_;
};

}




#endif