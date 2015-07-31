#include "rich_edit.h"
#include "helper.h"
#include "root_view.h"
#include "widget.h"
#include "logging.h"
#include "SkCanvas.h"

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
txt_bits_(TXTBIT_RICHTEXT | TXTBIT_MULTILINE | TXTBIT_WORDWRAP),
services_(nullptr), hwnd_(NULL)
{

}

RichEdit::~RichEdit()
{
    if (services_)
        services_->Release();

    if (dc_)
        ::DeleteDC(dc_);
}

void RichEdit::setText(const wchar_t * text)
{
    lazyInitialize();
    services_->TxSetText(text);
}

void RichEdit::setRichText(bool rich)
{
    lazyInitialize();
    if (rich)
        txt_bits_ |= TXTBIT_RICHTEXT;
    else
        txt_bits_ &= ~TXTBIT_RICHTEXT;
    services_->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, rich ? TXTBIT_RICHTEXT : 0);
}

bool RichEdit::getRichText() const
{
    return !!(txt_bits_ & TXTBIT_RICHTEXT);
}

void RichEdit::setReadOnly(bool readonly)
{
    lazyInitialize();
    if (readonly)
        txt_bits_ |= TXTBIT_READONLY;
    else
        txt_bits_ &= ~TXTBIT_READONLY;

    services_->OnTxPropertyBitsChange(TXTBIT_READONLY, readonly ? TXTBIT_READONLY : 0);
}

bool RichEdit::getReadOnly() const
{
    return !!(txt_bits_ & TXTBIT_READONLY);
}

void RichEdit::setLimitText(uint64_t length)
{
    lazyInitialize();
    if (max_length_ != (DWORD)length)
    {
        max_length_ = (DWORD)length;
        services_->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
    }
}

uint64_t RichEdit::getLimitText() const
{
    return max_length_;
}

void RichEdit::setWordWrap(bool wordwrap)
{
    lazyInitialize();
    if (wordwrap)
        txt_bits_ |= TXTBIT_WORDWRAP;
    else
        txt_bits_ &= ~TXTBIT_WORDWRAP;
    services_->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, wordwrap ? TXTBIT_WORDWRAP : 0);
}

bool RichEdit::getWordWrap() const
{
    return !!(txt_bits_ & TXTBIT_WORDWRAP);
}

const char * RichEdit::getClassName() const
{
    return kClassRichEdit;
}

HRESULT STDMETHODCALLTYPE RichEdit::QueryInterface(
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
    return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE RichEdit::AddRef(void)
{
    retain();
    return getRefCount();
}

ULONG STDMETHODCALLTYPE RichEdit::Release(void)
{
    auto count = getRefCount();
    release();
    count--;
    return count;
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
    Rect bounds;
    if (prc)
    {
        bounds = Helper::ToRect(*prc);
        inval(bounds);
    }    
    else
    {
        getLocalBounds(bounds);
        inval();
    }  
    dirty_.join(bounds);   
}

void RichEdit::TxViewChange(BOOL fUpdate)
{
    Rect bounds;
    getLocalBounds(bounds);
    dirty_.join(bounds);
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
    return ::SetCaretPos(static_cast<INT>(pt.x()), static_cast<INT>(pt.y()));
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
    notifyChangeCursor(hcur);
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
    if (COLOR_WINDOW == nIndex)
        return RGB(255, 255, 255);

    return ::GetSysColor(nIndex);
}

HRESULT	RichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    *pstyle = TXTBACK_TRANSPARENT;
    *pstyle = TXTBACK_OPAQUE;  
    return S_OK;
}

HRESULT	RichEdit::TxGetMaxLength(DWORD *plength)
{
    *plength = (DWORD)max_length_;
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

    lpExtent->cx = DXtoHimetricX((LONG)getWidth(), xPerInch);
    lpExtent->cy = DYtoHimetricY((LONG)getHeight(), yPerInch);

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

void RichEdit::onDraw(SkCanvas & canvas)
{
    lazyInitialize();
    adjustSurface();
    if (surface_.isEmpty() || !surface_.isValid())
        return;
    if (!dirty_.isEmpty())
    {
        auto old = ::SelectObject(dc_, Helper::ToHBitmap(surface_));
        Rect bounds;
        getLocalBounds(bounds);
        auto wbounds = Helper::ToRect(bounds);
        RECTL rcL = { wbounds.left, wbounds.top, wbounds.right, wbounds.bottom };
        auto wdirty = Helper::ToRect(dirty_);
        services_->TxDraw(
            DVASPECT_CONTENT,
            0,
            NULL,
            NULL,
            dc_,
            NULL,
            &rcL,
            NULL,
            &wdirty,
            NULL,
            NULL,
            TXTVIEW_ACTIVE);
        
        ::SelectObject(dc_, old);
        dirty_.setEmpty();
    }
    canvas.drawBitmap(Helper::ToSkBitmap(surface_), 0, 0);
}

void RichEdit::adjustSurface()
{
    int iwidth = (int)getWidth();
    int iheight = (int)getHeight();

    if (surface_.getWidth() != iwidth ||
        surface_.getHeight() != iheight)
    {
        surface_.allocate(iwidth, iheight);
        assert(surface_.isValid());
        surface_.erase(0xffffffff);
    }
}

HWND RichEdit::getHWND()
{
    if (!hwnd_)
    {
        auto rv = getRoot();
        if (rv)
        {
            auto widget = rv->getWidget();
            if (widget)
                hwnd_ = widget->hwnd();
        }
    }
    assert(hwnd_);
    if (!hwnd_)
        LOG_ERROR << "rich edit did't attach to a root view ?";
    return hwnd_;
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
    memset(&cf_, 0, sizeof(cf_));
    cf_.cbSize = sizeof(CHARFORMAT2);

    hwnd = GetDesktopWindow();
    hdc = GetDC(hwnd);
    yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
    cf_.yHeight = lf.lfHeight * LY_PER_INCH / yPixPerInch;
    ReleaseDC(hwnd, hdc);

    cf_.yOffset = 0;
    cf_.crTextColor = -1;

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

    //cf_.dwMask = CFM_ALL | CFM_BACKCOLOR | CFM_COLOR;
    cf_.dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

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

void RichEdit::lazyInitialize()
{
    typedef HRESULT (STDAPICALLTYPE * T_CreateTextServices)(
        IUnknown *punkOuter,
        ITextHost *pITextHost,
        IUnknown **ppUnk);

    if (!dc_)
        dc_ = ::CreateCompatibleDC(NULL);
    assert(dc_);
    if (!services_)
    {
        HMODULE rich = ::GetModuleHandle(L"Msftedit.dll");
        if (!rich)
            rich = ::LoadLibrary(L"Msftedit.dll");
        assert(rich);
        if (!rich)
        {
            LOG_ERROR << "rich edit Msftedit.dll not found";
            return;
        }
            
        initDefaultCF();
        initDefaultPF();

        IUnknown * pun = nullptr;
        T_CreateTextServices TCreateTextServices = (T_CreateTextServices)GetProcAddress(rich,
            "CreateTextServices");
        IID* IID_ITS = (IID*)(VOID*)GetProcAddress(rich,
            "IID_ITextServices");

        auto hr = TCreateTextServices(NULL, this, &pun);
        if (FAILED(hr))
        {
            LOG_ERROR << "rich edit CreateTextServices fail";
            return;
        }

        hr = pun->QueryInterface(*IID_ITS, (void **)&services_);
        pun->Release();
        if (FAILED(hr))
        {
            LOG_ERROR << "rich edit QueryInterface IID_ITextServices fail";
            return;
        }
        assert(services_);
        services_->OnTxInPlaceActivate(NULL);
    }
           
}

}