#ifndef BONES_CORE_VIEW_H_
#define BONES_CORE_VIEW_H_

#include "ref.h"
#include "point.h"
#include "size.h"
#include "accelerator.h"
#include <vector>
#include "css_types.h"

class SkCanvas;

namespace bones
{

class Rect;
class Root;

class View : public Ref, public AcceleratorTarget
{
public:
    typedef std::vector<View *> Views;
private:
    struct Flag
    {
        uint32_t visible : 1;
        uint32_t focusable : 1;
        uint32_t mouseable : 1;//true则接收target鼠标事件
        uint32_t clip : 1;//true 则子孙全部限制在自己的范围之内
    };
public:
    View();

    virtual ~View();

    void setOpacity(float opacity);//0~1.f

    float getOpacity() const;

    void setLoc(Scalar x, Scalar y);

    void setSize(Scalar w, Scalar h);

    void setLoc(const Point & pt);

    void setSize(const Size & size);

    const Size & getSize() const;

    const Point & getLoc() const;

    void setLeft(Scalar left);

    void setTop(Scalar top);

    void setWidth(Scalar width);

    void setHeight(Scalar height);

    Scalar getLeft() const;

    Scalar getTop() const;

    Scalar getWidth() const;

    Scalar getHeight() const;

    void setClip(bool clip);

    void setVisible(bool visible);

    void setFocusable(bool focusable);

    void setMouseable(bool mouseable);

    bool visible() const;

    bool focusable() const;

    bool mouseable() const;

    bool clip() const;

    void inval();

    void inval(const Rect & r);

    void attachChildToFront(View * child);

    void attachChildToBack(View * child);

    void attachChildAt(View * child, size_t index);

    void detachChild(View * child);

    void detachChildren();

    void detachFromParent();

    void recursiveDetachChildren();

    size_t getChildCount() const;

    View * getChildAt(size_t index) const;

    View * getFirstChild() const;

    View * getNextSibling() const;

    View * getPrevSibling() const;

    View * getLastChild() const;

    View * parent() const;

    virtual Root * getRoot();

    void getGlobalBounds(Rect & rect) const;

    void getLocalBounds(Rect & rect) const;

    Point mapToGlobal(const Point & loc) const;

    Point mapToLocal(const Point & global) const;

    View * hitTest(const Point & pt);

    bool contains(const Point & pt) const;

    bool contains(View * v) const;

    int group() const;

    void getViewsInGroup(int gid, Views & views) const;

    View * getSelectedForGroup(int group_id) const;

    View * getPrevFocusable() const;

    View * getNextFocusable() const;

    void requestFocus();

    void requestTop();

    void setCursor(Cursor cursor);

    void showCaret(bool show);

    void setCaretPos(const Point & pt);

    void setCaretSize(const Size & size);

    void createCaret();

    void destroyCaret();

    virtual bool isGroupFocusTraversable() const;

    virtual bool skipDefaultKeyEventProcessing(const KeyEvent & ke);

    virtual bool isVisible() const;

    virtual bool isFocusable() const;

    virtual bool isClip() const;
    //快捷键处理
    bool process(const Accelerator & accelerator) override;

    bool canProcess() const override;

    virtual const char * getClassName() const;
protected:
    void dispatch(Event & e);

    void draw(SkCanvas & canvas, const Rect & inval, float opacity);
protected:
    virtual bool onHitTest(const Point & pt);

    virtual void onDraw(SkCanvas & canvas, const Rect & inval, float opacity);

    virtual void onDrawCaret(SkCanvas & canvas, const Rect & inval, 
                             const Size & size, float opacity);

    virtual void onSizeChanging(Size & size);

    virtual void onSizeChanged();

    virtual void onPositionChanging(Point & pt);

    virtual void onPositionChanged();

    virtual void onVisibleChanged(View * start);

    virtual void onAddHierarchy(View * start);

    virtual void onRemoveHierarchy(View * start);
protected:
    virtual void onMouseEnter(MouseEvent & e);

    virtual void onMouseMove(MouseEvent & e);

    virtual void onMouseDown(MouseEvent & e);

    virtual void onMouseUp(MouseEvent & e);

    virtual void onMouseClick(MouseEvent & e);

    virtual void onMouseDClick(MouseEvent & e);

    virtual void onMouseLeave(MouseEvent & e);

    virtual void onKeyDown(KeyEvent & e);

    virtual void onKeyUp(KeyEvent & e);

    virtual void onChar(KeyEvent & e);

    virtual void onCompositionStart(CompositionEvent & e);

    virtual void onCompositionUpdate(CompositionEvent & e);

    virtual void onCompositionEnd(CompositionEvent & e);

    virtual void onFocusOut(FocusEvent & e);

    virtual void onFocusIn(FocusEvent & e);

    virtual void onBlur(FocusEvent & e);

    virtual void onFocus(FocusEvent & e);

    virtual void onWheel(WheelEvent & e);
protected:
    virtual bool notifyRemoveHierarchy(View * n);

    virtual bool notifyAddHierarchy(View * n);

    virtual bool notifyVisibleChanged(View * n);

    virtual bool notifyInval(const Rect & inval);

    virtual bool notifySetFocus(View * n);

    virtual bool notifyRegAccelerator(View * n, const Accelerator & accelerator);

    virtual bool notifyUnregAccelerator(View * n, const Accelerator & accelerator);

    virtual bool notifyChangeCursor(View * n, Cursor cursor);

    virtual bool notifyShowCaret(View * n, bool show);

    virtual bool notifyChangeCaretPos(View * n, const Point & pt);

    virtual bool notifyChangeCaretSize(View * n, const Size & size);

    virtual bool notifyCreateCaret(View * n);

    virtual bool notifyDestroyCaret(View * n);
private:
    void insertFocusSblings(View * v, int index);

    void eraseFocusSblings(View * v);

    void recursiveVisibleNotifications(View * start);

    void recursiveAddNotifications(View * start);

    void recursiveRemoveNotifications(View * start);

    void propagateRemoveNotifications();

    void propagateAddNotifications();

    void propagateVisibleNotifications();

    BONES_CSS_TABLE_DECLARE()

    void setLeft(const CSSParams & params);

    void setTop(const CSSParams & params);

    void setWidth(const CSSParams & params);

    void setHeight(const CSSParams & params);

    void setLoc(const CSSParams & params);

    void setSize(const CSSParams & params);

    void setFlag(const CSSParams & params);

    void setCursor(const CSSParams & params);

    void setOpacity(const CSSParams & params);
protected:
    Flag flag_;
    Point loc_;
    Size size_;

    RefPtr<View> parent_;
    RefPtr<View> first_child_;//环形链表
    RefPtr<View> prev_sibling_;
    RefPtr<View> next_sibling_;
    size_t children_count_;

    RefPtr<View> next_focusable_;
    RefPtr<View> prev_focusable_;
    int tag_;
    int group_id_;

    float opacity_;
    friend class EventDispatcher;
    friend class Root;
};

}
#endif