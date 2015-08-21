#ifndef BONES_CORE_FOCUS_CONTROLLER_H_
#define BONES_CORE_FOCUS_CONTROLLER_H_

#include "ref.h"

namespace bones
{
class Root;
class View;
class KeyEvent;

class FocusController
{
    enum FocusChangeReason
    {
        kTraversal,

        kRestore,

        kDirectChange,
    };
public:
    FocusController(Root * root);

    ~FocusController();

    bool handleKeyEvent(const KeyEvent & ke);

    View * current() const;

    void removed(View * n);

    void shiftIfNecessary();

    void shift(View * n);
private:
    bool isTabTraversalKeyEvent(const KeyEvent & ke);

    bool isArrowKeyEvent(const KeyEvent & ke);

    void advanceFocus(bool reverse);

    void advanceFocusInGroup(bool reverse);

    bool processArrowKeyTraversal(const KeyEvent & ke);

    void clearFocusIfNecessary();

    void setFocusWithReason(View * view, FocusChangeReason reason);

    bool isFocusable(View * v);

    bool isFocusableCandidate(View * v, int skip_group_id);

    View * findSelectedForGroup(View * v);
private:
    View * findNextFocusable(View * start, bool reverse);

    View * findNextFocusable(View * start,
                             bool check_start,
                             bool can_go_up,
                             bool can_go_down,
                             int skip_group_id);

    View * findPrevFocusable(View * start,
                             bool check_start,
                             bool can_go_up,
                             bool can_go_down,
                             int skip_group_id);


private:
    Root * root_;
    RefPtr<View> current_;
    RefPtr<View> stored_;
    FocusChangeReason reason_;
    bool arrow_key_traversal_enabled_;
};

}

#endif