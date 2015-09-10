#ifndef BONES_CORE_RICH_EDIT_H_
#define BONES_CORE_RICH_EDIT_H_

#include "view.h"
#include "pixmap.h"
#include "rect.h"
#include <Richedit.h>
#include <Textserv.h>
#include <functional>

namespace bones
{

class Font;
class Animation;

class RichEdit : public View, public ITextHost
{
public:
    class Delegate
    {
    public:
        virtual BOOL screenToClient(RichEdit * sender, LPPOINT lppt) = 0;

        virtual BOOL clientToScreen(RichEdit * sender, LPPOINT lppt) = 0;

        virtual HIMC immGetContext(RichEdit * sender) = 0;

        virtual void immReleaseContext(RichEdit * sender, HIMC himc) = 0;

        virtual HRESULT txNotify(RichEdit * sender, DWORD iNotify, void  *pv) = 0;

        virtual void onReturn(RichEdit * sender) = 0;
    };

    enum Want
    {
        kNone = 0,
        kTab = 1 << 0,
        kReturn = 1 << 1,//单行模式下按enter 有return通知
    };
public:
    RichEdit();

    ~RichEdit();

    void setDelegate(Delegate * delegate);

    void setWant(uint64_t want);
    
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
    //ITextHost
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)override;

    virtual ULONG STDMETHODCALLTYPE AddRef(void) override;

    virtual ULONG STDMETHODCALLTYPE Release(void) override;
    //@cmember Get the DC for the host
    HDC TxGetDC() override;

    //@cmember Release the DC gotten from the host
    INT TxReleaseDC(HDC hdc) override;

    //@cmember Show the scroll bar
    BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override;

    //@cmember Enable the scroll bar
    BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override;

    //@cmember Set the scroll range
    BOOL TxSetScrollRange(INT fnBar, 
                          LONG nMinPos, 
                          INT nMaxPos, 
                          BOOL fRedraw) override;

    //@cmember Set the scroll position
    BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override;

    //@cmember InvalidateRect
    void TxInvalidateRect(LPCRECT prc, BOOL fMode) override;

    //@cmember Send a WM_PAINT to the window
    virtual void TxViewChange(BOOL fUpdate) override;

    //@cmember Create the caret
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override;

    //@cmember Show the caret
    virtual BOOL TxShowCaret(BOOL fShow) override;

    //@cmember Set the caret position
    virtual BOOL TxSetCaretPos(INT x, INT y) override;

    //@cmember Create a timer with the specified timeout
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override;

    //@cmember Destroy a timer
    virtual void TxKillTimer(UINT idTimer) override;

    //@cmember Scroll the content of the specified window's client area
    virtual void TxScrollWindowEx(
                                  INT dx,
                                  INT dy,
                                  LPCRECT lprcScroll,
                                  LPCRECT lprcClip,
                                  HRGN hrgnUpdate,
                                  LPRECT lprcUpdate,
                                  UINT fuScroll) override;

    //@cmember Get mouse capture
    virtual void TxSetCapture(BOOL fCapture) override;

    //@cmember Set the focus to the text window
    virtual void TxSetFocus() override;

    //@cmember Establish a new cursor shape
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText) override;

    //@cmember Converts screen coordinates of a specified point to the client coordinates
    virtual BOOL TxScreenToClient(LPPOINT lppt) override;

    //@cmember Converts the client coordinates of a specified point to screen coordinates
    virtual BOOL TxClientToScreen(LPPOINT lppt) override;

    //@cmember Request host to activate text services
    virtual HRESULT	TxActivate(LONG * plOldState) override;

    //@cmember Request host to deactivate text services
    virtual HRESULT	TxDeactivate(LONG lNewState) override;

    //@cmember Retrieves the coordinates of a window's client area
    virtual HRESULT	TxGetClientRect(LPRECT prc) override;

    //@cmember Get the view rectangle relative to the inset
    virtual HRESULT	TxGetViewInset(LPRECT prc) override;

    //@cmember Get the default character format for the text
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF) override;

    //@cmember Get the default paragraph format for the text
    virtual HRESULT	TxGetParaFormat(const PARAFORMAT **ppPF) override;

    //@cmember Get the background color for the window
    virtual COLORREF TxGetSysColor(int nIndex) override;

    //@cmember Get the background (either opaque or transparent)
    virtual HRESULT	TxGetBackStyle(TXTBACKSTYLE *pstyle) override;

    //@cmember Get the maximum length for the text
    virtual HRESULT	TxGetMaxLength(DWORD *plength) override;

    //@cmember Get the bits representing requested scroll bars for the window
    virtual HRESULT	TxGetScrollBars(DWORD *pdwScrollBar) override;

    //@cmember Get the character to display for password input
    virtual HRESULT	TxGetPasswordChar(TCHAR *pch) override;

    //@cmember Get the accelerator character
    virtual HRESULT	TxGetAcceleratorPos(LONG *pcp) override;

    //@cmember Get the native size
    virtual HRESULT	TxGetExtent(LPSIZEL lpExtent) override;

    //@cmember Notify host that default character format has changed
    virtual HRESULT OnTxCharFormatChange(const CHARFORMATW * pcf) override;

    //@cmember Notify host that default paragraph format has changed
    virtual HRESULT	OnTxParaFormatChange(const PARAFORMAT * ppf) override;

    //@cmember Bulk access to bit properties
    virtual HRESULT	TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) override;

    //@cmember Notify host of events
    virtual HRESULT	TxNotify(DWORD iNotify, void *pv) override;

    // Far East Methods for getting the Input Context
    //#ifdef WIN95_IME
    virtual HIMC TxImmGetContext() override;

    virtual void TxImmReleaseContext(HIMC himc) override;
    //#endif

    //@cmember Returns HIMETRIC size of the control bar.
    virtual HRESULT	TxGetSelectionBarWidth(LONG *lSelBarWidth) override;
protected:
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

    void onAddHierarchy(View * start) override;

    bool skipDefaultKeyEventProcessing(const KeyEvent & ke) override;
protected:
    BONES_CSS_TABLE_DECLARE()

    void setText(const CSSParams & params);

    void setBackground(const CSSParams & params);

    void setFont(const CSSParams & params);
private:
    void adjustSurface();

    void initDefaultCF();

    void initDefaultPF();

    void lazyInitialize();

    void preprocessSurface(Pixmap & render, Pixmap & update);

    void postprocessSurface(Pixmap & update);

    void onAnimationRun(Animation * sender, Ref * target, float progress);
private:
    Delegate * delegate_;
    HDC dc_;
    HGDIOBJ old_obj_;
    CHARFORMAT2 cf_;
    PARAFORMAT pf_;
    DWORD max_length_;
    wchar_t password_;
    DWORD scroll_bars_;//srollbar的标志 跟窗口样式有关
    uint64_t txt_bits_;//TXT_BIT_*
    LONG accel_pos_;
    ITextServices * services_;
    Surface surface_;
    bool bg_opaque_;
    Color bg_color_;
    bool bg_set_color_;
    bool traversal_;

    uint64_t want_;
    std::map<UINT, Animation*> animations_;
};

}

#endif