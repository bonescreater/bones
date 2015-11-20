#ifndef BONES_SAMPLE_PANEL_H_
#define BONES_SAMPLE_PANEL_H_

#include "bones/bones.h"
#include <Windows.h>

class BSPanel : public BonesRoot::NotifyListener
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

    void shiftFocus(BonesRoot * sender, BonesObject * prev, 
        BonesObject * current, bool & stop)  override;

    void invalidRect(BonesRoot * sender, const BonesRect & rect, bool & stop) override;

    void changeCursor(BonesRoot * sender, BonesCursor cursor, bool & stop) override;

    void onSizeChanged(BonesRoot * sender, const BonesSize & size, bool & stop) override;

    void onPositionChanged(BonesRoot * sender, const BonesPoint & loc, bool & stop) override;

    bool onHitTest(BonesRoot * sender, const BonesPoint & loc, bool & stop) override;
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
    bool ime_start_;
private:
    friend LRESULT CALLBACK BSPanelProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam);
};

#endif