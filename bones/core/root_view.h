#ifndef BONES_CORE_ROOT_VIEW_H_
#define BONES_CORE_ROOT_VIEW_H_

#include "view.h"
#include "accelerator_manager.h"
#include "mouse_controller.h"
#include "focus_controller.h"
#include "pixmap.h"
#include "rect.h"

class SkBaseDevice;

namespace bones
{

class Widget;

struct NativeEvent
{
    UINT msg;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT result;
};

class RootView : public View
{
public:
    class Delegate
    {
    public:
        virtual Widget * getWidget() = 0;

        virtual void requestFocus() = 0;

        virtual void invalidateRect(const Rect & rect) = 0;

        virtual void changeCursor(Cursor cursor) = 0;
    };
public:
    RootView();

    ~RootView();

    void setDelegate(Delegate * delegate);

    void draw();

    const Rect & getDirtyRect() const;

    bool isDirty() const;

    Surface & getBackBuffer();

    Widget * getWidget() const;

    void handleMouse(NativeEvent & e);

    void handleKey(NativeEvent & e);

    void handleFocus(NativeEvent & e);

    void handleComposition(NativeEvent & e);

    void handleWheel(NativeEvent & e);

    bool isVisible() const override;

    RootView * getRoot() override;

    const char * getClassName() const override;
protected:
    //void handleEvent(WheelEvent & e);

    //void handleEvent(Event & e);
protected:
    virtual void onDraw(SkCanvas & canvas, const Rect & inval) override;

    virtual void onVisibleChanged(View * start) override;

    virtual bool notifyInval(const Rect & inval) override;

    virtual bool notifyAddHierarchy(View * n) override;

    virtual bool notifyRemoveHierarchy(View * n) override;

    virtual bool notifyVisibleChanged(View * n) override;

    virtual bool notifySetFocus(View * n) override;

    virtual bool notifyChangeCursor(Cursor cursor) override;
private:
    void AdjustPixmap();
private:
    FocusController focus_;
    MouseController mouse_;
    AcceleratorManager accelerators_;
    Surface back_buffer_;
    SkBaseDevice * device_;
    Delegate * delegate_;
    Rect dirty_;
    bool has_focus_;
    friend class MouseController;
};


}
#endif