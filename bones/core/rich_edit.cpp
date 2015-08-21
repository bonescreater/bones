#include "rich_edit.h"
#include "helper.h"
#include "root.h"
#include "logging.h"
#include "SkCanvas.h"
#include "device.h"

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

void UpdateCHARFORMAT2(LOGFONT & lf, CHARFORMAT2 & cf)
{
    HWND hwnd;
    HDC hdc;
    LONG yPixPerInch;

    hwnd = GetDesktopWindow();
    hdc = GetDC(hwnd);
    yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
    cf.yHeight = lf.lfHeight * LY_PER_INCH / yPixPerInch;
    ReleaseDC(hwnd, hdc);

    cf.dwEffects = CFM_EFFECTS;
    cf.dwEffects &= ~(CFE_PROTECTED | CFE_LINK | CFE_AUTOCOLOR);

    if (lf.lfWeight < FW_BOLD)
        cf.dwEffects &= ~CFE_BOLD;
    if (!lf.lfItalic)
        cf.dwEffects &= ~CFE_ITALIC;
    if (!lf.lfUnderline)
        cf.dwEffects &= ~CFE_UNDERLINE;
    if (!lf.lfStrikeOut)
        cf.dwEffects &= ~CFE_STRIKEOUT;

    cf.dwMask = CFM_ALL;

    cf.bCharSet = lf.lfCharSet;
    cf.bPitchAndFamily = lf.lfPitchAndFamily;
    memcpy(cf.szFaceName, lf.lfFaceName, LF_FACESIZE * sizeof(cf.szFaceName[0]));
    //wcscpy(cf.szFaceName, L"Microsoft Yahei");
}

RichEdit::RichEdit()
:accel_pos_(-1), max_length_(INFINITE), password_(L'*'),
scroll_bars_(WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
ES_AUTOHSCROLL | ES_DISABLENOSCROLL), 
txt_bits_(TXTBIT_RICHTEXT | TXTBIT_MULTILINE | TXTBIT_WORDWRAP),
services_(nullptr), delegate_(nullptr),
bg_opaque_(true), bg_color_(0xff0000ff), bg_set_color_(true),
dc_(NULL), traversal_(false)
{
    lazyInitialize();
}

RichEdit::~RichEdit()
{
    if (dc_)
        ::DeleteDC(dc_);
    if (services_)
        services_->Release();
}

void RichEdit::setText(const wchar_t * text)
{
    services_->TxSetText(text);
}

void RichEdit::setRichText(bool rich)
{
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

void RichEdit::setBackground(bool opaque, Color * bg_color)
{
    bg_opaque_ = opaque;
    bg_set_color_ = false;
    if (bg_color)
    {
        bg_color_ = *bg_color;
        bg_set_color_ = true;
    }
    services_->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, TXTBIT_BACKSTYLECHANGE);
}

void RichEdit::setFont(const wchar_t * family, int text_size, bool bBold, bool bUnderline, bool bItalic)
{
    LOGFONT lf = { 0 };
    ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
    if (family)
    {
        size_t len = LF_FACESIZE;
        size_t family_len = wcslen(family) + 1;//0结尾
        len = family_len < len ? family_len : len;
        memcpy(lf.lfFaceName, family, len * sizeof(wchar_t));
    }
    
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = text_size;
    if (bBold) 
        lf.lfWeight += FW_BOLD;
    if (bUnderline) 
        lf.lfUnderline = TRUE;
    if (bItalic) 
        lf.lfItalic = TRUE;
    HFONT hfont = ::CreateFontIndirect(&lf);
    if (hfont)
    {
        ::GetObject(hfont, sizeof(LOGFONT), &lf);
        // Set CHARFORMAT structure
        UpdateCHARFORMAT2(lf, cf_);
        ::DeleteFont(hfont);
        services_->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE,
            TXTBIT_CHARFORMATCHANGE);
    }
}

const char * RichEdit::getClassName() const
{
    return kClassRichEdit;
}

void RichEdit::onDraw(SkCanvas & canvas, const Rect & inval)
{
    adjustSurface();
    if (surface_.isEmpty() || !surface_.isValid())
        return;

    auto & target = Device::From(canvas.getDevice());
    auto update = surface_.extractSubset(inval);
    preprocessSurface(target, update);

    Rect bounds;
    getLocalBounds(bounds);
    auto wbounds = Helper::ToRect(bounds);
    RECTL rcL = { wbounds.left, wbounds.top, wbounds.right, wbounds.bottom };
    RECT winval = Helper::ToRect(inval);

    //只更新脏区
    services_->TxDraw(
        DVASPECT_CONTENT,
        0,
        NULL,
        NULL,
        Helper::ToHDC(surface_),
        NULL,
        &rcL,
        NULL,
        &winval,
        NULL,
        NULL,
        TXTVIEW_ACTIVE);

    postprocessSurface(update);
    SkPaint paint;
    //paint.setAntiAlias(true);
    //paint.setFilterLevel(SkPaint::kHigh_FilterLevel);
    paint.setAlpha(ClampAlpha(opacity_));
    canvas.drawBitmap(Helper::ToSkBitmap(surface_), 0, 0, &paint);
}

void RichEdit::onPositionChanged()
{
    services_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
}

void RichEdit::onSizeChanged()
{
    services_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
}

void RichEdit::onMouseMove(MouseEvent & e)
{
    LRESULT lr = 1;
    auto root = getRoot();
    //WM_SETCURSOR仅在非Capture下WM_MOUSEMOVE才会发送
    //模拟WINDOWS 的SetCursor
    if (NULL == ::GetCapture())
    {
        auto & loc = e.getLoc();
        services_->OnTxSetCursor(DVASPECT_CONTENT, 0, NULL, NULL,
            dc_, NULL, NULL, (INT)loc.x(), (INT)loc.y());
    }


    services_->TxSendMessage(
        Helper::ToMsgForMouse(e.type(), e.button()),
        Helper::ToKeyStateForMouse(e.getFlags()), 
        Helper::ToCoordinateForMouse(e.getLoc()), &lr);
}

void RichEdit::onMouseDown(MouseEvent & e)
{
    services_->TxSendMessage(
        Helper::ToMsgForMouse(e.type(), e.button()),
        Helper::ToKeyStateForMouse(e.getFlags()),
        Helper::ToCoordinateForMouse(e.getLoc()), nullptr);
}

void RichEdit::onMouseUp(MouseEvent & e)
{
    services_->TxSendMessage(
        Helper::ToMsgForMouse(e.type(), e.button()),
        Helper::ToKeyStateForMouse(e.getFlags()),
        Helper::ToCoordinateForMouse(e.getLoc()), nullptr);
}

void RichEdit::onFocus(FocusEvent & e)
{
    services_->OnTxInPlaceActivate(NULL);
    services_->TxSendMessage(WM_SETFOCUS, 0, 0, nullptr);
    traversal_ = e.isTabTraversal();
}

void RichEdit::onBlur(FocusEvent & e)
{
    services_->TxSendMessage(WM_KILLFOCUS, 0, 0, nullptr);
    services_->OnTxInPlaceActivate(NULL);
}

void RichEdit::onKeyDown(KeyEvent & e)
{
    switch (e.key())
    {
    case kVKEY_BACK:
    case kVKEY_RETURN:
    //case kVKEY_TAB: tab用于切换焦点
    case kVKEY_UP:
    case kVKEY_DOWN:
    case kVKEY_LEFT:
    case kVKEY_RIGHT:
        services_->TxSendMessage(WM_KEYDOWN, e.key(), Helper::ToKeyStateForKey(e.state()), 0);
    }
        
}

void RichEdit::onKeyPress(KeyEvent & e)
{
    if (e.ch() != '\t')
        services_->TxSendMessage(WM_CHAR, e.ch(), Helper::ToKeyStateForKey(e.state()), 0);
}

void RichEdit::onCompositionStart(CompositionEvent & e)
{
    auto native = (NativeEvent *)e.getUserData();
    if (native)
        services_->TxSendMessage(native->msg, native->wparam, native->lparam, 0);
}

void RichEdit::onCompositionUpdate(CompositionEvent & e)
{
    auto native = (NativeEvent *)e.getUserData();
    if (native)
        services_->TxSendMessage(native->msg, native->wparam, native->lparam, 0);
}

void RichEdit::onCompositionEnd(CompositionEvent & e)
{
    auto native = (NativeEvent *)e.getUserData();
    if (native)
        services_->TxSendMessage(native->msg, native->wparam, native->lparam, 0);
}

void RichEdit::onAddHierarchy(View * start)
{
    lazyInitialize();
}

bool RichEdit::skipDefaultKeyEventProcessing(const KeyEvent & ke)
{
    return false;
}

void RichEdit::adjustSurface()
{
    int iwidth = (int)getWidth();
    int iheight = (int)getHeight();

    if (surface_.getWidth() != iwidth ||
        surface_.getHeight() != iheight)
    {
        surface_.free();
        if (iwidth && iheight)
        {
            surface_.allocate(iwidth, iheight);
            assert(surface_.isValid());
        }
    }
}

void RichEdit::initDefaultCF()
{
    memset(&cf_, 0, sizeof(cf_));
    cf_.cbSize = sizeof(CHARFORMAT2);
    cf_.crTextColor = RGB(0xff, 0, 0);
    setFont(L"Microsoft Yahei", 20, false, false, false);  
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

void RichEdit::preprocessSurface(Pixmap & render, Pixmap & update)
{
    //GDI 在TxDraw里有文字抗锯齿 抗锯齿的像素来源于背景
    //所以在背景透明的情况下 从画布中读取背景像素用于抗锯齿
    //如果此时画布中的背景也是半透明 会导致 文字周边有阴影 
    //GDI暂时无法解决 等WIN8 WIN10流行后 换用TxDrawD2D应该可以解决掉这个问题
    if (bg_opaque_)
        return;
    Pixmap::LockRec sr;
    Pixmap::LockRec dr;
    render.lock(sr);
    update.lock(dr);
    auto dstart = (char *)dr.bits + 
        (int)(dr.subset.left() * 4 + dr.subset.top() * dr.pitch);
    auto sstart = (char *)sr.bits +
        (int)((sr.subset.left() + getLeft()) * 4 + (sr.subset.top() + getTop()) * sr.pitch);

    int dheight = (int)dr.subset.height();
    int dwidth = (int)dr.subset.width();

    for (auto i = 0; i < dheight; ++i)
    {
        for (auto j = 0; j < dwidth; ++j)
        {
            Color src = *((Color *)sstart + j);
            Color * pdst = (Color *)dstart + j;
            *pdst = src;
            //设置alpha为0xff
            *((char *)pdst + 3) = '\xff';
        }
        dstart += dr.pitch;
        sstart += sr.pitch;
    }

    update.unlock();
    render.unlock();
}

void RichEdit::postprocessSurface(Pixmap & update)
{
    Pixmap::LockRec lr;
    if (!update.lock(lr))
        return;

    char * cs = (char *)lr.bits + 
        (int)(lr.subset.left() * 4 + lr.subset.top() * lr.pitch);
    auto height = (int)lr.subset.height();
    auto width = (int)lr.subset.width();

    for (auto i = 0; i < height; ++i)
    {
        for (auto j = 0; j < width; ++j)
        {
            Color * pc = (Color *)cs + j;

            auto pa = ((char *)pc + 3);
            if (bg_opaque_)//不透明背景 所有像素有效 alpha全部置为255
                *pa = '\xff';
            else
            {//透明背景 清除所有的背景 保留文字
                if (*pa != 0)
                    *pc = 0;
                else
                     *pa = '\xff';               
            }
        }
        cs += lr.pitch;
    }
    update.unlock();
}

void RichEdit::lazyInitialize()
{
    typedef HRESULT(STDAPICALLTYPE * T_CreateTextServices)(
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
        initDefaultCF();
        initDefaultPF();
        services_->OnTxInPlaceActivate(NULL);
    }
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
    return dc_;
}

INT RichEdit::TxReleaseDC(HDC hdc)
{
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
    assert(delegate_);
    return delegate_ ? delegate_->createCaret(this, hbmp, xWidth, yHeight) : 0;   
}

BOOL RichEdit::TxShowCaret(BOOL fShow)
{
    return delegate_ ? delegate_->showCaret(this, fShow) : 0;
}

BOOL RichEdit::TxSetCaretPos(INT x, INT y)
{
    auto pt = mapToGlobal(Point::Make(static_cast<Scalar>(x), 
                                      static_cast<Scalar>(y)));

    return ::SetCaretPos(static_cast<INT>(pt.x()), static_cast<INT>(pt.y()));
}

BOOL RichEdit::TxSetTimer(UINT idTimer, UINT uTimeout)
{
    return delegate_?delegate_->setTimer(this, idTimer, uTimeout) : 0;
}

void RichEdit::TxKillTimer(UINT idTimer)
{
    delegate_ ? delegate_->killTimer(this, idTimer) : 0;
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
    POINT screen = Helper::ToPoint(mapToLocal(Helper::ToPoint(*lppt)));
    auto ret = delegate_ ? delegate_->screenToClient(this, &screen) : 0;
    if (ret)
        *lppt = screen;
    return ret;
}

BOOL RichEdit::TxClientToScreen(LPPOINT lppt)
{
    if (!lppt)
        return TRUE;
    POINT client = Helper::ToPoint(mapToGlobal(Helper::ToPoint(*lppt)));
    auto ret = delegate_ ? delegate_->clientToScreen(this, &client) : 0;
    if (ret)
        *lppt = client;
    return ret;
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
    if (COLOR_WINDOW == nIndex && bg_set_color_)
        return RGB(ColorGetR(bg_color_), ColorGetG(bg_color_), ColorGetB(bg_color_));
        

    return ::GetSysColor(nIndex);
}

HRESULT	RichEdit::TxGetBackStyle(TXTBACKSTYLE *pstyle)
{
    if (bg_opaque_)
        *pstyle = TXTBACK_OPAQUE;
    else
        *pstyle = TXTBACK_TRANSPARENT;    
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
    auto dc = Helper::ToHDC(surface_);
    LONG xPerInch = ::GetDeviceCaps(dc, LOGPIXELSX);
    LONG yPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);

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
    return delegate_ ? delegate_->immGetContext(this) : 0;
}

void RichEdit::TxImmReleaseContext(HIMC himc)
{
    delegate_ ? delegate_->immReleaseContext(this, himc) : 0;
}

HRESULT	RichEdit::TxGetSelectionBarWidth(LONG *lSelBarWidth)
{
    *lSelBarWidth = 0;
    return S_OK;
}



}