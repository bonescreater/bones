#ifndef BONES_SAMPLE_PANEL_H_
#define BONES_SAMPLE_PANEL_H_

#include "bones/bones.h"
#include <Windows.h>

class BSPanel : public BonesRoot::NotifyListener, 
                public BonesRichEdit::NotifyListener
{
public:
    static bool Initialize();

    static bool Uninitialize();

    BSPanel();

    ~BSPanel();

    bool create(const BSPanel * parent, bool layered);

    bool destroy();

    void attach(HWND hwnd);

    void detach();

    HWND hwnd() const;

    bool setLoc(const POINT & pt);

    bool setSize(const SIZE & size);

    void layeredDraw();

    bool isLayered() const;

    void loadRoot(BonesRoot * root);

    void onCreate(BonesRoot * sender, bool & stop) override;

    void onDestroy(BonesRoot * sender, bool & stop) override;

    void requestFocus(BonesRoot * sender, bool & stop) override;

    void invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop) override;

    void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) override;

    void showCaret(BonesRoot * sender, bool fshow, bool & stop) override;

    void createCaret(BonesRoot * sender, BonesCaret hbmp, const BonesSize & size, bool & stop) override;

    void changeCaretPos(BonesRoot * sender, const BonesPoint & pt, bool & stop) override;

    void onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop) override;

    void onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop) override;

    void onCreate(BonesRichEdit * sender, bool & stop) override;

    void onDestroy(BonesRichEdit * sender, bool & stop) override;

    HRESULT txNotify(BonesRichEdit * sender, DWORD iNotify, void  *pv, bool & stop) override;

    void onReturn(BonesRichEdit * sender, bool & stop) override;

    BOOL screenToClient(BonesRichEdit * sender, LPPOINT lppt, bool & stop) override;

    BOOL clientToScreen(BonesRichEdit * sender, LPPOINT lppt, bool & stop) override;

    HIMC immGetContext(BonesRichEdit * sender, bool & stop) override;

    void immReleaseContext(BonesRichEdit * sender, HIMC himc, bool & stop);
protected:
    void onPaint(HDC hdc, const RECT & rect);

    void onGeometryChanged(WINDOWPOS & pos);

    LRESULT processEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT defProcessEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleCreate(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handlePositionChanges(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleFocus(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handlePaint(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleKey(UINT uMsg, WPARAM wParam, LPARAM lParam);

    LRESULT handleIME(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
    HWND hwnd_;
    HDC dc_;
    BonesRoot * root_;
    bool track_mouse_;
    BonesCursor cursor_;
    uint64_t ex_style_;
private:
    friend LRESULT CALLBACK BSPanelProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam);
};

#endif