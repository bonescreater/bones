#ifndef BONES_CORE_WEB_VIEW_H_
#define BONES_CORE_WEB_VIEW_H_

#include "area.h"
#include "pixmap.h"


namespace bones
{

class Browser;

class WebView : public Area<WebView> 
{
public:
    class Delegate : public DelegateBase
    {

    };
public:
    static bool StartUp(const char * locate);

    static void ShutDown();

    static void Update();

    WebView();

    ~WebView();

    void setDelegate(Delegate * delegate);

    bool open();

    void close();

    void loadURL(const wchar_t * url);

    void executeJS(const wchar_t * code, const wchar_t * url, int start_line);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onSizeChanged() override;

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

    void onCompositionUpdate(CompositionEvent & e) override;

    bool skipDefaultKeyEventProcessing(const KeyEvent & ke) override;
private:
    void adjustPixmap();
private:
    Delegate * delegate_;
    Browser * browser_;
    bool hack_focus_travel_;
};

}




#endif