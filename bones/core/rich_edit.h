#ifndef BONES_CORE_RICH_EDIT_H_
#define BONES_CORE_RICH_EDIT_H_

#include "area.h"
#include "pixmap.h"
#include "rect.h"


namespace bones
{

class Font;
class Animation;
class TextHost;

class RichEdit : public Area<RichEdit>
{
public:
    class Delegate : public DelegateBase
    {
    public:
        virtual HRESULT txNotify(RichEdit * sender, DWORD iNotify, void  *pv) = 0;
    };
public:
    RichEdit();

    ~RichEdit();

    void setDelegate(Delegate * delegate);
    
    void setText(const wchar_t * text);

    void setRichText(bool rich);

    bool getRichText() const;

    void setReadOnly(bool readonly);

    bool getReadOnly() const;

    void setLimitText(uint64_t length);

    uint64_t getLimitText() const;

    void setWordWrap(bool wordwrap);

    bool getWordWrap() const;

    void setBackground(bool opaque, Color * bg_color);

    void setFont(const Font & font);

    const char * getClassName() const override;
protected:
    DelegateBase * delegate() override;

    void onDraw(SkCanvas & canvas, const Rect & inval, float opacity) override;

    void onPositionChanged() override;

    void onSizeChanged() override;

    void onMouseEnter(MouseEvent & e) override;

    void onMouseLeave(MouseEvent & e) override;

    void onMouseMove(MouseEvent & e) override;

    void onMouseDown(MouseEvent & e) override;

    void onMouseUp(MouseEvent & e) override;

    void onFocus(FocusEvent & e) override;

    void onBlur(FocusEvent & e) override;

    void onKeyDown(KeyEvent & e) override;

    void onKeyUp(KeyEvent & e) override;

    void onChar(KeyEvent & e) override;

    void onCompositionStart(CompositionEvent & e) override;

    void onCompositionUpdate(CompositionEvent & e) override;

    void onCompositionEnd(CompositionEvent & e) override;

    bool skipDefaultKeyEventProcessing(const KeyEvent & ke) override;
protected:
    BONES_CSS_TABLE_DECLARE()

    void setText(const CSSParams & params);

    void setBackground(const CSSParams & params);

    void setFont(const CSSParams & params);
private:
    void preprocessSurface(Pixmap & render, Pixmap & update);

    void postprocessSurface(Pixmap & update);
private:
    Delegate * delegate_;
    TextHost * host_;
    bool traversal_;
    bool ime_;
    friend class TextHost;
};

}

#endif