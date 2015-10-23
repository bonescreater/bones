#ifndef BONES_CORE_ROOT_H_
#define BONES_CORE_ROOT_H_

#include "area.h"
#include "accelerator_manager.h"
#include "mouse_controller.h"
#include "focus_controller.h"
#include "pixmap.h"
#include "rect.h"

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

class Root : public Area<Root>
{
public:
    class Delegate : public DelegateBase
    {
    public:
        virtual void requestFocus(Root * sender) = 0;

        virtual void invalidRect(Root * sender, const Rect & rect) = 0;

        virtual void changeCursor(Root * sender, Cursor cursor) = 0;
    };
public:
    Root();

    ~Root();

    void setDelegate(Delegate * delegate);

    void setColor(Color color);

    void attachTo(Widget widget);

    void draw();

    const Rect & getDirtyRect() const;

    bool isDirty() const;

    const Surface & getBackBuffer() const;

    void getBackBuffer(const void * & bits, size_t & pitch) const;

    bool handleMouse(NativeEvent & e);

    bool handleKey(NativeEvent & e);

    bool handleFocus(NativeEvent & e);

    bool handleComposition(NativeEvent & e);

    bool handleWheel(NativeEvent & e);

    Widget getWidget();

    bool isVisible() const override;

    Root * getRoot() override;

    const char * getClassName() const override;
protected:
    virtual void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    virtual void onSizeChanged() override;

    virtual void onPositionChanged() override;

    virtual void onVisibleChanged(View * start) override;

    virtual bool notifyInval(const Rect & inval) override;

    virtual bool notifyAddHierarchy(View * n) override;

    virtual bool notifyRemoveHierarchy(View * n) override;

    virtual bool notifyVisibleChanged(View * n) override;

    virtual bool notifySetFocus(View * n) override;

    virtual bool notifyChangeCursor(View * n, Cursor cursor) override;

    virtual bool notifyShowCaret(View * n, bool show) override;

    virtual bool notifyChangeCaretPos(const Point & pt) override;

    virtual bool notifyCreateCaret(Caret caret, const Size & size) override;
protected:
    DelegateBase * delegate() override;
private:
    void adjustPixmap();

    BONES_CSS_TABLE_DECLARE()

    void setColor(const CSSParams & params);
private:
    Delegate * delegate_;
    Widget widget_;
    FocusController focus_;
    MouseController mouse_;
    AcceleratorManager accelerators_;
    Surface back_buffer_;
    SkBaseDevice * device_;
    Rect dirty_;
    bool has_focus_;
    void * bits_;
    size_t pitch_;
    Color color_;
    friend class MouseController;
};


}
#endif