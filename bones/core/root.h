#ifndef BONES_CORE_ROOT_H_
#define BONES_CORE_ROOT_H_

#include "area.h"
#include "accelerator_manager.h"
#include "mouse_controller.h"
#include "focus_controller.h"
#include "pixmap.h"
#include "rect.h"
#include "animation.h"

class SkBaseDevice;

namespace bones
{

class Animation;

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
private:
    struct DirtyRect
    {
        enum Flag
        {
            kNone = 0,
            kView = 1 << 0,
            kCaret = 1 << 1,
        };
        int flag_;
        Rect rect;
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

    void sendMouse(MouseEvent & e);

    void sendKey(KeyEvent & e);

    void sendWheel(WheelEvent & e);

    void sendFocus(bool focus);

    bool sendComposition(CompositionEvent & e);

    Widget getWidget();

    void restoreCaret();

    void restoreCursor();

    void update();

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

    virtual bool notifyChangeCaretPos(View * n, const Point & pt) override;

    virtual bool notifyChangeCaretSize(View * n, const Size & size) override;

    virtual bool notifyCreateCaret(View * n) override;

    virtual bool notifyDestroyCaret(View * n) override;
private:
    void drawCaret(SkCanvas & canvas);

    void drawView(SkCanvas & canvas, const Rect & inval);

    Rect getCaretRect();

    void onCaretRun(Animation * sender, View * target, float progress);

    void onCaretStop(Animation * sender, View * target);

    void onCaretStart(Animation * sender, View * target);

    void invalView(const Rect & inval);

    void invalCaret();
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
    Surface view_buffer_;
    SkBaseDevice * view_device_;
    bool has_focus_;
    void * bits_;
    size_t pitch_;
    Color color_;
    DirtyRect dirty_;
    DirtyRect wait_dirty_;
    //caret support
    Point caret_loc_;
    Size caret_size_;
    bool caret_show_;
    bool caret_display_;
    bool force_caret_display_;//强制显示光标 为false 则按照caret_display显示
    Animation * caret_ani_;
    friend class MouseController;
};


}
#endif