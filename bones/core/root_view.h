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

    void setColor(Color color);

    void setOpacity(float opacity);

    float getOpacity() const;

    void draw();

    const Rect & getDirtyRect() const;

    bool isDirty() const;

    Surface & getBackBuffer();

    Widget * getWidget() const;

    void handleEvent(KeyEvent & e);

    void handleEvent(MouseEvent & e);

    void handleEvent(FocusEvent & e);

    //void handleEvent(WheelEvent & e);

    //void handleEvent(Event & e);

    void setFocus(bool focus);

    bool isVisible() const override;

    RootView * getRoot() override;

    const char * getClassName() const override;

protected:
    virtual void onDraw(SkCanvas & canvas) override;

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
    Color color_;
    float opacity_;

    friend class MouseController;
};


}
#endif