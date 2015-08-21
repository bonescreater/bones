#ifndef BONES_CORE_ROOT_H_
#define BONES_CORE_ROOT_H_

#include "view.h"
#include "accelerator_manager.h"
#include "mouse_controller.h"
#include "focus_controller.h"
#include "pixmap.h"
#include "rect.h"
#include <functional>

class SkBaseDevice;

namespace bones
{

struct NativeEvent
{
    UINT msg;
    WPARAM wparam;
    LPARAM lparam;
    LRESULT result;
};

class Root : public View
{
public:
    class Delegate
    {
    public:
        virtual void requestFocus(Ref * sender) = 0;

        virtual void invalidRect(Ref * sender, const Rect & rect) = 0;

        virtual void changeCursor(Ref * sender, Cursor cursor) = 0;
    };
public:
    Root();

    ~Root();

    void setDelegate(Delegate & delegate);

    void draw();

    const Rect & getDirtyRect() const;

    bool isDirty() const;

    Surface & getBackBuffer();

    void handleMouse(NativeEvent & e);

    void handleKey(NativeEvent & e);

    void handleFocus(NativeEvent & e);

    void handleComposition(NativeEvent & e);

    void handleWheel(NativeEvent & e);

    bool isVisible() const override;

    Root * getRoot() override;

    const char * getClassName() const override;
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
    Delegate * delegate_;

    FocusController focus_;
    MouseController mouse_;
    AcceleratorManager accelerators_;
    Surface back_buffer_;
    SkBaseDevice * device_;
    Rect dirty_;
    bool has_focus_;
    friend class MouseController;
};


}
#endif