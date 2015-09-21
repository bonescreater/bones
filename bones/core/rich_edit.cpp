#include "rich_edit.h"
#include "helper.h"
#include "root.h"
#include "logging.h"
#include "SkCanvas.h"
#include "device.h"
#include "font.h"
#include "encoding.h"
#include "animation.h"
#include "animation_manager.h"
#include "css_utils.h"

#include <Richedit.h>
#include <Textserv.h>
#include <windowsx.h>

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

/*
TextHost
*/

class TextHost : public ITextHost, public Ref
{
    //ITextHost
public:
    TextHost()
        :services_(nullptr), rich_(nullptr), dc_(NULL), accel_pos_(-1), 
        max_length_(INFINITE), password_(L'*'),
        scroll_bars_(WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
        ES_AUTOHSCROLL | ES_DISABLENOSCROLL),
        txt_bits_(TXTBIT_RICHTEXT | TXTBIT_MULTILINE | TXTBIT_WORDWRAP),
        bg_opaque_(true), bg_color_(0xff000088), bg_set_color_(true),
        old_obj_(NULL)
    {
        
    }

    ~TextHost()
    {
        if (dc_)
        {
            if (old_obj_)
                ::SelectObject(dc_, old_obj_);
            ::DeleteDC(dc_);
        }
        if (services_)
            services_->Release();
    }

    void setHost(RichEdit * re)
    {
        rich_ = re;
        lazyInitialize();
    }

    RichEdit::Delegate * delegate()
    {
        return rich_ ? rich_->delegate_ : nullptr ;
    }

    void lazyInitialize()
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
                BLG_ERROR << "rich edit Msftedit.dll not found";
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
                BLG_ERROR << "rich edit CreateTextServices fail";
                return;
            }

            hr = pun->QueryInterface(*IID_ITS, (void **)&services_);
            pun->Release();
            if (FAILED(hr))
            {
                BLG_ERROR << "rich edit QueryInterface IID_ITextServices fail";
                return;
            }
            assert(services_);
            initDefaultCF();
            initDefaultPF();
            services_->OnTxInPlaceActivate(NULL);
        }

    }
    void initDefaultCF()
    {
        memset(&cf_, 0, sizeof(cf_));
        cf_.cbSize = sizeof(CHARFORMAT2);
        cf_.crTextColor = RGB(0xff, 0, 0);
        setFont(Font());
    }

    void initDefaultPF()
    {
        memset(&pf_, 0, sizeof(PARAFORMAT));

        pf_.cbSize = sizeof(PARAFORMAT);
        pf_.dwMask = PFM_ALL;
        pf_.wAlignment = PFA_LEFT;
        pf_.cTabCount = 1;
        pf_.rgxTabs[0] = lDefaultTab;
    }

    void setFont(const Font & font)
    {
        LOGFONT lf = { 0 };
        ::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), &lf);
        if (font.getFamily())
        {
            auto family = Encoding::FromUTF8(font.getFamily());
            size_t len = LF_FACESIZE;
            size_t family_len = family.size() + 1;//0结尾
            len = family_len < len ? family_len : len;
            memcpy(lf.lfFaceName, family.data(), len * sizeof(wchar_t));
        }

        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = static_cast<LONG>(font.getSize());
        auto st = font.getStyle();
        if (Font::kBold & st)
            lf.lfWeight += FW_BOLD;
        if (Font::kUnderline & st)
            lf.lfUnderline = TRUE;
        if (Font::kItalic & st)
            lf.lfItalic = TRUE;
        if (Font::kStrikeOut & st)
            lf.lfStrikeOut = TRUE;

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

    void adjustSurface()
    {
        if (!rich_)
            return;
        int iwidth = (int)rich_->getWidth();
        int iheight = (int)rich_->getHeight();

        if (surface_.getWidth() != iwidth ||
            surface_.getHeight() != iheight)
        {
            if (old_obj_)
                ::SelectObject(dc_, old_obj_);
            surface_.free();
            if (iwidth && iheight)
            {
                surface_.allocate(iwidth, iheight);
                assert(surface_.isValid());
                old_obj_ = ::SelectObject(dc_, Helper::ToHBITMAP(surface_));
            }
        }
    }
public:
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)override
    {
        return E_NOTIMPL;
    }

    virtual ULONG STDMETHODCALLTYPE AddRef(void)
    {
        retain();
        return getRefCount();
    }

    virtual ULONG STDMETHODCALLTYPE Release(void)
    {
        auto count = getRefCount();
        release();
        count--;
        return count;
    }
    //@cmember Get the DC for the host
    HDC TxGetDC() override
    {
        return dc_;
    }

    //@cmember Release the DC gotten from the host
    INT TxReleaseDC(HDC hdc) override
    {
        return 1;
    }

    //@cmember Show the scroll bar
    BOOL TxShowScrollBar(INT fnBar, BOOL fShow) override
    {
        return TRUE;
    }

    //@cmember Enable the scroll bar
    BOOL TxEnableScrollBar(INT fuSBFlags, INT fuArrowflags) override
    {
        return TRUE;
    }

    //@cmember Set the scroll range
    BOOL TxSetScrollRange(INT fnBar,
        LONG nMinPos,
        INT nMaxPos,
        BOOL fRedraw) override
    {
        return TRUE;
    }

    //@cmember Set the scroll position
    BOOL TxSetScrollPos(INT fnBar, INT nPos, BOOL fRedraw) override
    {
        return TRUE;
    }

    //@cmember InvalidateRect
    void TxInvalidateRect(LPCRECT prc, BOOL fMode) override
    {
        if (prc)
            rich_ ? rich_->inval(Helper::ToRect(*prc)) : 0;
        else
            rich_ ? rich_->inval() : 0;
    }

    //@cmember Send a WM_PAINT to the window
    virtual void TxViewChange(BOOL fUpdate) override
    {
        rich_ ? rich_->inval() : 0;
    }

    //@cmember Create the caret
    virtual BOOL TxCreateCaret(HBITMAP hbmp, INT xWidth, INT yHeight) override
    {
        rich_ ? rich_->createCaret(hbmp,
            Size::Make(static_cast<Scalar>(xWidth), 
            static_cast<Scalar>(yHeight))):0;
        return TRUE;
    }

    //@cmember Show the caret
    virtual BOOL TxShowCaret(BOOL fShow) override
    {
        rich_ ? rich_->showCaret(!!fShow) : 0;
        return TRUE;
    }

    //@cmember Set the caret position
    virtual BOOL TxSetCaretPos(INT x, INT y) override
    {
        rich_ ? rich_->setCaretPos(
            Point::Make(static_cast<Scalar>(x), 
                        static_cast<Scalar>(y))) : 0;
        return TRUE;
    }

    //@cmember Create a timer with the specified timeout
    virtual BOOL TxSetTimer(UINT idTimer, UINT uTimeout) override
    {
        TxKillTimer(idTimer);
        if (!rich_)
            return TRUE;;
        auto ani = new Animation(rich_, uTimeout, -1, (void *)idTimer);
        ani->bind(BONES_CLASS_CALLBACK_3(&TextHost::onAnimationRun, this));
        rich_->animations_[idTimer] = ani;
        Core::GetAnimationManager()->add(ani);

        return TRUE;
    }

    //@cmember Destroy a timer
    virtual void TxKillTimer(UINT idTimer) override
    {
        if (!rich_)
            return;
        auto iter = rich_->animations_.find(idTimer);
        if (iter != rich_->animations_.end())
        {
            Core::GetAnimationManager()->remove(iter->second, false);
            iter->second->release();
            rich_->animations_.erase(iter);
        }
    }

    //@cmember Scroll the content of the specified window's client area
    virtual void TxScrollWindowEx(
        INT dx,
        INT dy,
        LPCRECT lprcScroll,
        LPCRECT lprcClip,
        HRGN hrgnUpdate,
        LPRECT lprcUpdate,
        UINT fuScroll) override
    {

    }

    //@cmember Get mouse capture
    virtual void TxSetCapture(BOOL fCapture) override
    {

    }

    //@cmember Set the focus to the text window
    virtual void TxSetFocus() override
    {
        rich_ ? rich_->requestFocus() : 0;
    }

    //@cmember Establish a new cursor shape
    virtual void TxSetCursor(HCURSOR hcur, BOOL fText) override
    {
        rich_ ? rich_->setCursor(hcur) : 0;
    }

    //@cmember Converts screen coordinates of a specified point to the client coordinates
    virtual BOOL TxScreenToClient(LPPOINT lppt) override
    {
        if (!lppt)
            return TRUE;
        if (!rich_)
            return TRUE;

        POINT screen = Helper::ToPoint(rich_->mapToLocal(Helper::ToPoint(*lppt)));
        auto ret = delegate() ? delegate()->screenToClient(rich_, &screen) : 0;
        if (ret)
            *lppt = screen;
        return ret;
    }

    //@cmember Converts the client coordinates of a specified point to screen coordinates
    virtual BOOL TxClientToScreen(LPPOINT lppt) override
    {
        if (!lppt)
            return TRUE;
        if (!rich_)
            return TRUE;
        POINT client = Helper::ToPoint(rich_->mapToGlobal(Helper::ToPoint(*lppt)));
        auto ret = delegate() ? delegate()->clientToScreen(rich_, &client) : 0;
        if (ret)
            *lppt = client;
        return ret;
    }

    //@cmember Request host to activate text services
    virtual HRESULT	TxActivate(LONG * plOldState) override
    {
        return S_OK;
    }

    //@cmember Request host to deactivate text services
    virtual HRESULT	TxDeactivate(LONG lNewState) override
    {
        return S_OK;
    }

    //@cmember Retrieves the coordinates of a window's client area
    virtual HRESULT	TxGetClientRect(LPRECT prc) override
    {
        Rect local;
        rich_ ? rich_->getLocalBounds(local) : 0;
        *prc = Helper::ToRect(local);
        return S_OK;
    }

    //@cmember Get the view rectangle relative to the inset
    virtual HRESULT	TxGetViewInset(LPRECT prc) override
    {
        prc->left = 0;
        prc->top = 0;
        prc->right = 0;
        prc->bottom = 0;
        return S_OK;
    }

    //@cmember Get the default character format for the text
    virtual HRESULT TxGetCharFormat(const CHARFORMATW **ppCF) override
    {
        *ppCF = &cf_;
        return S_OK;
    }

    //@cmember Get the default paragraph format for the text
    virtual HRESULT	TxGetParaFormat(const PARAFORMAT **ppPF) override
    {
        *ppPF = &pf_;
        return S_OK;
    }

    //@cmember Get the background color for the window
    virtual COLORREF TxGetSysColor(int nIndex) override
    {
        //COLOR_WINDOW
        if (COLOR_WINDOW == nIndex && bg_set_color_)
            return RGB(ColorGetR(bg_color_), ColorGetG(bg_color_), ColorGetB(bg_color_));


        return ::GetSysColor(nIndex);
    }

    //@cmember Get the background (either opaque or transparent)
    virtual HRESULT	TxGetBackStyle(TXTBACKSTYLE *pstyle) override
    {
        if (bg_opaque_)
            *pstyle = TXTBACK_OPAQUE;
        else
            *pstyle = TXTBACK_TRANSPARENT;
        return S_OK;
    }

    //@cmember Get the maximum length for the text
    virtual HRESULT	TxGetMaxLength(DWORD *plength) override
    {
        *plength = (DWORD)max_length_;
        return S_OK;
    }

    //@cmember Get the bits representing requested scroll bars for the window
    virtual HRESULT	TxGetScrollBars(DWORD *pdwScrollBar) override
    {
        *pdwScrollBar = scroll_bars_ & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
            ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

        return S_OK;
    }

    //@cmember Get the character to display for password input
    virtual HRESULT	TxGetPasswordChar(TCHAR *pch) override
    {
        *pch = password_;
        return S_OK;
    }

    //@cmember Get the accelerator character
    virtual HRESULT	TxGetAcceleratorPos(LONG *pcp) override
    {
        *pcp = accel_pos_;
        return S_OK;
    }

    //@cmember Get the native size
    virtual HRESULT	TxGetExtent(LPSIZEL lpExtent) override
    {
        LONG xPerInch = ::GetDeviceCaps(dc_, LOGPIXELSX);
        LONG yPerInch = ::GetDeviceCaps(dc_, LOGPIXELSY);

        lpExtent->cx = DXtoHimetricX(
            (LONG)(rich_ ? rich_->getWidth() : 0), xPerInch);
        lpExtent->cy = DYtoHimetricY(
            (LONG)(rich_ ? rich_->getHeight() : 0), yPerInch);

        return S_OK;
    }

    //@cmember Notify host that default character format has changed
    virtual HRESULT OnTxCharFormatChange(const CHARFORMATW * pcf) override
    {
        return S_OK;
    }

    //@cmember Notify host that default paragraph format has changed
    virtual HRESULT	OnTxParaFormatChange(const PARAFORMAT * ppf) override
    {
        pf_ = *ppf;
        return S_OK;
    }
    //@cmember Bulk access to bit properties
    virtual HRESULT	TxGetPropertyBits(DWORD dwMask, DWORD *pdwBits) override
    {
            *pdwBits = txt_bits_ & dwMask;
            return S_OK;
    }

    //@cmember Notify host of events
    virtual HRESULT	TxNotify(DWORD iNotify, void *pv) override
    {
        return delegate() ? delegate()->txNotify(rich_, iNotify, pv) : S_OK;
    }

    // Far East Methods for getting the Input Context
    //#ifdef WIN95_IME
    virtual HIMC TxImmGetContext() override
    {
        return delegate() ? delegate()->immGetContext(rich_) : 0;
    }

    virtual void TxImmReleaseContext(HIMC himc) override
    {
        delegate() ? delegate()->immReleaseContext(rich_, himc) : 0;
    }
    //#endif

    //@cmember Returns HIMETRIC size of the control bar.
    virtual HRESULT	TxGetSelectionBarWidth(LONG *lSelBarWidth) override
    {
        *lSelBarWidth = 0;
        return S_OK;
    }
    void onAnimationRun(Animation * sender, Ref * target, float progress)
    {
        assert(sender);
        if (sender)
            services_->TxSendMessage(WM_TIMER, (WPARAM)sender->userData(), 0, nullptr);
    }
private:
    RichEdit * rich_;
    HDC dc_;
    CHARFORMAT2 cf_;
    PARAFORMAT pf_;
    Color bg_color_;
    bool bg_set_color_;
    bool bg_opaque_;
    DWORD max_length_;
    DWORD scroll_bars_;//srollbar的标志 跟窗口样式有关
    wchar_t password_;
    LONG accel_pos_;
    uint64_t txt_bits_;//TXT_BIT_*
    ITextServices * services_;
    HGDIOBJ old_obj_;
    Surface surface_;
    friend class RichEdit;
};


RichEdit::RichEdit()
:delegate_(nullptr), traversal_(false), ime_(false)
{
    host_ = new TextHost();
    host_->setHost(this);
}

RichEdit::~RichEdit()
{
    //移除当前所有的定时器
    for (auto iter = animations_.begin(); iter != animations_.end(); ++iter)
    {
        Core::GetAnimationManager()->remove(iter->second, false);
        iter->second->release();
    }
    host_->setHost(nullptr);
    host_->release();
}

void RichEdit::setDelegate(Delegate * delegate)
{
    delegate_ = delegate;
}

void RichEdit::setText(const wchar_t * text)
{
    host_->services_->TxSetText(text);
    inval();
}

void RichEdit::setRichText(bool rich)
{
    if (rich)
        host_->txt_bits_ |= TXTBIT_RICHTEXT;
    else
        host_->txt_bits_ &= ~TXTBIT_RICHTEXT;
    host_->services_->OnTxPropertyBitsChange(TXTBIT_RICHTEXT, rich ? TXTBIT_RICHTEXT : 0);
}

bool RichEdit::getRichText() const
{
    return !!(host_->txt_bits_ & TXTBIT_RICHTEXT);
}

void RichEdit::setReadOnly(bool readonly)
{
    if (readonly)
        host_->txt_bits_ |= TXTBIT_READONLY;
    else
        host_->txt_bits_ &= ~TXTBIT_READONLY;

    host_->services_->OnTxPropertyBitsChange(TXTBIT_READONLY, readonly ? TXTBIT_READONLY : 0);
}

bool RichEdit::getReadOnly() const
{
    return !!(host_->txt_bits_ & TXTBIT_READONLY);
}

void RichEdit::setLimitText(uint64_t length)
{
    if (host_->max_length_ != (DWORD)length)
    {
        host_->max_length_ = (DWORD)length;
        host_->services_->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
    }
}

uint64_t RichEdit::getLimitText() const
{
    return host_->max_length_;
}

void RichEdit::setWordWrap(bool wordwrap)
{
    if (wordwrap)
        host_->txt_bits_ |= TXTBIT_WORDWRAP;
    else
        host_->txt_bits_ &= ~TXTBIT_WORDWRAP;
    host_->services_->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, wordwrap ? TXTBIT_WORDWRAP : 0);
}

bool RichEdit::getWordWrap() const
{
    return !!(host_->txt_bits_ & TXTBIT_WORDWRAP);
}

void RichEdit::setBackground(bool opaque, Color * bg_color)
{
    host_->bg_opaque_ = opaque;
    host_->bg_set_color_ = false;
    if (bg_color)
    {
        host_->bg_color_ = *bg_color;
        host_->bg_set_color_ = true;
    }
    host_->services_->OnTxPropertyBitsChange(TXTBIT_BACKSTYLECHANGE, TXTBIT_BACKSTYLECHANGE);
}

void RichEdit::setFont(const Font & font)
{
    host_->setFont(font);
}

const char * RichEdit::getClassName() const
{
    return kClassRichEdit;
}

void RichEdit::onDraw(SkCanvas & canvas, const Rect & inval, float opacity)
{
    host_->adjustSurface();
    if (0 == opacity)
        return;

    if (host_->surface_.isEmpty() || !host_->surface_.isValid())
        return;

    auto & target = Device::From(canvas.getDevice());
    auto update = host_->surface_.extractSubset(inval);
    preprocessSurface(target, update);

    Rect bounds;
    getLocalBounds(bounds);
    auto wbounds = Helper::ToRect(bounds);
    RECTL rcL = { wbounds.left, wbounds.top, wbounds.right, wbounds.bottom };
    RECT winval = Helper::ToRect(inval);
    //只更新脏区
    host_->services_->TxDraw(
        DVASPECT_CONTENT,
        0,
        NULL,
        NULL,
        host_->dc_,
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
    paint.setAlpha(ClampAlpha(opacity));
    canvas.drawBitmap(Helper::ToSkBitmap(host_->surface_), 0, 0, &paint);
}

void RichEdit::onPositionChanged()
{
    host_->services_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
}

void RichEdit::onSizeChanged()
{
    host_->services_->OnTxPropertyBitsChange(TXTBIT_CLIENTRECTCHANGE, TRUE);
}

void RichEdit::onMouseEnter(MouseEvent & e)
{
    host_->services_->OnTxInPlaceActivate(NULL);
}

void RichEdit::onMouseLeave(MouseEvent & e)
{
    //services_->OnTxInPlaceDeactivate();
}

void RichEdit::onMouseMove(MouseEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    LRESULT lr = 1;
    auto root = getRoot();
    //WM_SETCURSOR仅在非Capture下WM_MOUSEMOVE才会发送
    //模拟WINDOWS 的SetCursor
    if (!e.isLeftMouse())
    {
        auto & loc = e.getLoc();
        host_->services_->OnTxSetCursor(DVASPECT_CONTENT, 0, NULL, NULL,
            host_->dc_, NULL, NULL, (INT)loc.x(), (INT)loc.y());
    }


    host_->services_->TxSendMessage(
        Helper::ToMsgForMouse(e.type(), e.button()),
        Helper::ToKeyStateForMouse(e.getFlags()), 
        Helper::ToCoordinateForMouse(e.getLoc()), &lr);
}

void RichEdit::onMouseDown(MouseEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    host_->services_->TxSendMessage(
        Helper::ToMsgForMouse(e.type(), e.button()),
        Helper::ToKeyStateForMouse(e.getFlags()),
        Helper::ToCoordinateForMouse(e.getLoc()), nullptr);
}

void RichEdit::onMouseUp(MouseEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    host_->services_->TxSendMessage(
        Helper::ToMsgForMouse(e.type(), e.button()),
        Helper::ToKeyStateForMouse(e.getFlags()),
        Helper::ToCoordinateForMouse(e.getLoc()), nullptr);
}

void RichEdit::onFocus(FocusEvent & e)
{
    //焦点到richedit时处于PlaceActivate
    if (Event::kTarget != e.phase())
        return;
    host_->services_->OnTxInPlaceActivate(NULL);
    host_->services_->OnTxUIActivate();
    host_->services_->TxSendMessage(WM_SETFOCUS, 0, 0, nullptr);
    traversal_ = e.isTabTraversal();
}

void RichEdit::onBlur(FocusEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    host_->services_->TxSendMessage(WM_KILLFOCUS, 0, 0, nullptr);
    if (ime_)
    {
        ime_ = false;
        host_->services_->TxSendMessage(WM_IME_ENDCOMPOSITION, 0, 0, 0);
    }    
    host_->services_->OnTxUIDeactivate();
    //services_->OnTxInPlaceDeactivate(NULL);
    
    
}

void RichEdit::onKeyDown(KeyEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    switch (e.key())
    {
    case kVKEY_RETURN:
    {
        if (!(host_->txt_bits_ & TXTBIT_MULTILINE))
        {//单行模式下
            delegate_ ? delegate_->onReturn(this) : 0;
            break;
        }
    }      
    case kVKEY_BACK:   
    case kVKEY_UP:
    case kVKEY_DOWN:
    case kVKEY_LEFT:
    case kVKEY_RIGHT:
        host_->services_->TxSendMessage(WM_KEYDOWN, e.key(), Helper::ToKeyStateForKey(e.state()), 0);
    }
        
}

void RichEdit::onKeyUp(KeyEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    switch (e.key())
    {
    case kVKEY_RETURN:
    {
        if (!(host_->txt_bits_ & TXTBIT_MULTILINE))
        {//单行模式下 而且需要return
            break;
        }
    }
    case kVKEY_BACK:
    case kVKEY_UP:
    case kVKEY_DOWN:
    case kVKEY_LEFT:
    case kVKEY_RIGHT:
        host_->services_->TxSendMessage(WM_KEYUP, e.key(), Helper::ToKeyStateForKey(e.state()), 0);
    }
}

void RichEdit::onChar(KeyEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    if (e.ch() != '\t' )
        host_->services_->TxSendMessage(WM_CHAR, e.ch(), Helper::ToKeyStateForKey(e.state()), 0);
}

void RichEdit::onCompositionStart(CompositionEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    ime_ = true;
    auto native = (NativeEvent *)e.getUserData();
    if (native)
        host_->services_->TxSendMessage(native->msg, native->wparam, native->lparam, 0);
}

void RichEdit::onCompositionUpdate(CompositionEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    auto native = (NativeEvent *)e.getUserData();
    if (native)
        host_->services_->TxSendMessage(native->msg, native->wparam, native->lparam, 0);
}

void RichEdit::onCompositionEnd(CompositionEvent & e)
{
    if (Event::kTarget != e.phase())
        return;
    ime_ = false;
    auto native = (NativeEvent *)e.getUserData();
    if (native)
        host_->services_->TxSendMessage(native->msg, native->wparam, native->lparam, 0);
}

bool RichEdit::skipDefaultKeyEventProcessing(const KeyEvent & ke)
{
    return false;
}

void RichEdit::preprocessSurface(Pixmap & render, Pixmap & update)
{
    //GDI 在TxDraw里有文字抗锯齿 抗锯齿的像素来源于背景
    //所以在背景透明的情况下 从画布中读取背景像素用于抗锯齿
    //如果此时画布中的背景也是半透明 会导致 文字周边有阴影 
    //GDI暂时无法解决 等WIN8 WIN10流行后 换用TxDrawD2D应该可以解决掉这个问题
    if (host_->bg_opaque_)
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
            if (host_->bg_opaque_)//不透明背景 所有像素有效 alpha全部置为255
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

BONES_CSS_TABLE_BEGIN(RichEdit, View)
BONES_CSS_SET_FUNC("content", &RichEdit::setText)
BONES_CSS_SET_FUNC("background-style", &RichEdit::setBackground)
BONES_CSS_SET_FUNC("font", &RichEdit::setFont)
BONES_CSS_TABLE_END()

void RichEdit::setText(const CSSParams & params)
{
    if (params.empty())
        return;
    CSSText content(params[0]);
    setText(Encoding::FromUTF8(content.begin, content.length).data());
}

void RichEdit::setBackground(const CSSParams & params)
{
    if (params.empty())
        return; 
    bool opaque = true;
    if (params[0] == "transparent")
        opaque = false;
    Color * c = nullptr;
    Color color = 0;
    if (params.size() > 1)
    {
        color = CSSUtils::CSSStrToColor(params[1]);
        c = &color;
    }
    setBackground(opaque, c);
}

void RichEdit::setFont(const CSSParams & params)
{
    if (params.empty())
        return;
    setFont(CSSUtils::CSSParamsToFont(params));
}

}