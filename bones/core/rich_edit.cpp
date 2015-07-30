#include "rich_edit.h"
#include "helper.h"
#include "root_view.h"
#include "widget.h"
#include "logging.h"

namespace bones
{

#define HIMETRIC_PER_INCH 2540
#define LY_PER_INCH   1440

LONG DYtoHimetricY(LONG dy, LONG yPerInch)
{
    return (LONG)MulDiv(dy, HIMETRIC_PER_INCH, yPerInch);
}

LONG DXtoHimetricX(LONG dx, LONG xPerInch)
{
    return (LONG)MulDiv(dx, HIMETRIC_PER_INCH, xPerInch);
}

RichEdit::RichEdit()
:dc_(NULL), accel_pos_(-1), max_length_(INFINITE), password_(L'*'),
scroll_bars_(WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
ES_AUTOHSCROLL | ES_DISABLENOSCROLL), 
txt_bits_(TXTBIT_RICHTEXT | TXTBIT_MULTILINE | TXTBIT_WORDWRAP)
{

}

RichEdit::~RichEdit()
{
    if (dc_)
        ::DeleteDC(dc_);
}

HDC RichEdit::TxGetDC()
{
    if (!dc_)
        dc_ = ::CreateCompatibleDC(NULL);
    return dc_;
}

INT RichEdit::TxReleaseDC(HDC hdc)
{
    assert(dc_ == hdc);
    return 1;
}

BOOL RichEdit::TxShowScrollBar(INT fnBar, BOOL fShow)
{
    return TRUE;
}

BOOL RichEdit::TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags)
{
    return TRUE;
}

BOOL RichEdit::TxSetScrollRange(INT fnBar,
                                LONG nMinPos,
                                INT nMaxPos,
                                BOOL fRedraw)

{
    return TRUE;
}

BOOL RichEdit::TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw)
{
    return TRUE;
}

void RichEdit::TxInvalidateRect(LPCRECT prc, BOOL fMode)
{
    if (prc)
        inval(Helper::ToRect(*prc));
    else
        inval();
}

void RichEdit::TxViewChange(BOOL fUpdate)
{
    inval();
}

BOOL RichEdit::TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight)
{
    return ::CreateCaret(getHWND(), hbmp, xWidth, yHeight);
}

BOOL RichEdit::TxShowCaret(BOOL fShow)
{
    if (fShow)
        return ::ShowCaret(getHWND());
    else
        return ::HideCaret(getHWND());
}

BOOL RichEdit::TxSetCaretPos(INT x, INT y)
{
    auto pt = mapToGlobal(Point::Make(static_cast<Scalar>(x), 
                                      static_cast<Scalar>(y)));
    ::SetCaretPos(static_cast<INT>(pt.x()), static_cast<INT>(pt.y()));
}

BOOL RichEdit::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    return ::SetTimer(getHWND(), idTimer, uTimeout, NULL);
}

void RichEdit::TxKillTimer(UINT idTimer)
{
    ::KillTimer(getHWND(), idTimer);
}

void RichEdit::TxScrollWindowEx(
                                INT dx,
                                INT dy,
                                LPCRECT lprcScroll,
                                LPCRECT lprcClip,
                                HRGN hrgnUpdate,
                                LPRECT lprcUpdate,
                                UINT fuScroll)
{
    ;
}

void RichEdit::TxSetCapture(BOOL fCapture)
{//默认已经capture了
    ;
}

void RichEdit::TxSetFocus()
{//request focus
    requestFocus();
}

void RichEdit::TxSetCursor(HCURSOR hcur, BOOL fText)
{
    ::SetCursor(hcur);
}

BOOL RichEdit::TxScreenToClient(LPPOINT lppt)
{
    if (!lppt)
        return TRUE;

    return ::ScreenToClient(getHWND(), 
        &Helper::ToPoint(mapToLocal(Helper::ToPoint(*lppt))));
}

BOOL RichEdit::TxClientToScreen(LPPOINT lppt)
{
    if (!lppt)
        return TRUE;

    return ::ClientToScreen(getHWND(),
        &Helper::ToPoint(mapToGlobal(Helper::ToPoint(*lppt))));
}

HRESULT	RichEdit::TxActivate(LONG * plOldState)
{
    return S_OK;
}

HRESULT	RichEdit::TxDeactivate(LONG lNewState)
{
    return S_OK;
}

HRESULT	RichEdit::TxGetClientRect(LPRECT prc)
{
    Rect local;
    getLocalBounds(local);
    *prc = Helper::ToRect(local);
    return S_OK;
}

HRESULT	RichEdit::TxGetViewInset(LPRECT prc)
{
    prc->left = 0;
    prc->top = 0;
    prc->right = 0;
    prc->bottom = 0;
    return S_OK;
}

HRESULT RichEdit::TxGetCharFormat(const CHARFORMATW **ppCF)
{
    *ppCF = &cf_;
    return S_OK;
}

HRESULT RichEdit::TxGetParaFormat(const PARAFORMAT **ppPF)
{
    *ppPF = &pf_;
    return S_OK;
}

COLORREF RichEdit::TxGetSysColor(int nIndex)
{
    //COLOR_WINDOW
    return ::GetSysColor(nIndex);
}

HRESULT	RichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = TXTBACK_TRANSPARENT;
    return S_OK;
}

HRESULT	RichEdit::TxGetMaxLength(DWORD *plength)
{
    *plength = max_length_;
    return S_OK;
}

HRESULT	RichEdit::TxGetScrollBars(DWORD *pdwScrollBar)
{
    *pdwScrollBar = scroll_bars_ & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return S_OK;
}

HRESULT	RichEdit::TxGetPasswordChar(TCHAR *pch)
{
    *pch = password_;
    return S_OK;
}

HRESULT	RichEdit::TxGetAcceleratorPos(LONG *pcp)
{
    *pcp = accel_pos_;
    return S_OK;
}

HRESULT	RichEdit::TxGetExtent(LPSIZEL lpExtent)
{//zooming
    LONG xPerInch = ::GetDeviceCaps(dc_, LOGPIXELSX);
    LONG yPerInch = ::GetDeviceCaps(dc_, LOGPIXELSY);

    lpExtent->cx = DXtoHimetricX(getWidth(), xPerInch);
    lpExtent->cy = DYtoHimetricY(getHeight(), yPerInch);

    return S_OK;
    //return E_NOTIMPL;
}

HRESULT RichEdit::OnTxCharFormatChange(const CHARFORMATW * pcf)
{
    return S_OK;
}

HRESULT	RichEdit::OnTxParaFormatChange(const PARAFORMAT * ppf)
{
    pf_ = *ppf;
    return S_OK;
}

HRESULT	RichEdit::TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits)
{
    *pdwBits = txt_bits_ & dwMask;
    return S_OK;
}

HRESULT	RichEdit::TxNotify(DWORD iNotify, void *pv)
{
    return S_OK;
}

HIMC RichEdit::TxImmGetContext()
{
    return ::ImmGetContext(getHWND());
}

void RichEdit::TxImmReleaseContext(HIMC himc)
{
    ::ImmReleaseContext(getHWND(), himc);
}

HRESULT	RichEdit::TxGetSelectionBarWidth(LONG *lSelBarWidth)
{
    *lSelBarWidth = 0;
    return S_OK;
}

HWND RichEdit::getHWND()
{
    HWND hwnd = NULL;
    auto rv = getRoot();
    if (rv)
    {
        auto widget = rv->getWidget();
        if (widget)
            hwnd = widget->hwnd();
    }
    assert(hwnd);
    if (!hwnd)
        LOG_VERBOSE << "rich edit did't attach to a root view ?";
    return hwnd;
}

void RichEdit::initDefaultCF()
{
    HWND hwnd;
    LOGFONT lf;
    HDC hdc;
    LONG yPixPerInch;
    HFONT hfont = NULL;

    // Get LOGFONT for default font
    if (!hfont)
        hfont = (HFONT)GetStockObject(SYSTEM_FONT);

    // Get LOGFONT for passed hfont
    if (!GetObject(hfont, sizeof(LOGFONT), &lf))
        return;

    // Set CHARFORMAT structure
    cf_.cbSize = sizeof(CHARFORMAT2);

    hwnd = GetDesktopWindow();
    hdc = GetDC(hwnd);
    yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
    cf_.yHeight = lf.lfHeight * LY_PER_INCH / yPixPerInch;
    ReleaseDC(hwnd, hdc);

    cf_.yOffset = 0;
    cf_.crTextColor = GetSysColor(COLOR_WINDOWTEXT);

    cf_.dwEffects = CFM_EFFECTS | CFE_AUTOBACKCOLOR;
    cf_.dwEffects &= ~(CFE_PROTECTED | CFE_LINK);

    if (lf.lfWeight < FW_BOLD)
        cf_.dwEffects &= ~CFE_BOLD;
    if (!lf.lfItalic)
        cf_.dwEffects &= ~CFE_ITALIC;
    if (!lf.lfUnderline)
        cf_.dwEffects &= ~CFE_UNDERLINE;
    if (!lf.lfStrikeOut)
        cf_.dwEffects &= ~CFE_STRIKEOUT;

    cf_.dwMask = CFM_ALL | CFM_BACKCOLOR;
    cf_.bCharSet = lf.lfCharSet;
    cf_.bPitchAndFamily = lf.lfPitchAndFamily;
    wcscpy(cf_.szFaceName, lf.lfFaceName);
}

void RichEdit::initDefaultPF()
{
    memset(&pf_, 0, sizeof(PARAFORMAT));

    pf_.cbSize = sizeof(PARAFORMAT);
    pf_.dwMask = PFM_ALL;
    pf_.wAlignment = PFA_LEFT;
    pf_.cTabCount = 1;
    pf_.rgxTabs[0] = lDefaultTab;
}

}