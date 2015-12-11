
#include "env_unittest.h"
#include "focus_controller.h"
#include "root.h"

namespace bones
{

TEST(FocusControllerUnitTest, CheckConstructor)
{
    Root * root = new Root;
    FocusController fc(root);
    EXPECT_EQ(root, fc.root_);
    EXPECT_EQ(nullptr, fc.current_);
    EXPECT_EQ(nullptr, fc.wait_focus_);
    EXPECT_FALSE(fc.has_focus_);
    EXPECT_EQ(FocusController::kTraversal, fc.reason_);
    EXPECT_FALSE(fc.arrow_key_traversal_enabled_);
    root->release();
}

TEST(FocusControllerUnitTest, DirectShift)
{
    Root * root = new Root;

    View * v1 = new View;
    root->attachChildToBack(v1);

    FocusController fc(root);
    //在hasfocus为false的情况下不能直接设置焦点
    EXPECT_FALSE(fc.hasFocus());
    EXPECT_EQ(nullptr, fc.current());
    fc.shift(v1);
    EXPECT_EQ(nullptr, fc.current());
    //设置后v3直接获得焦点
    fc.setFocus(true);
    EXPECT_EQ(v1, fc.current());
    //killfocus后 失去当前焦点
    fc.setFocus(false);
    EXPECT_EQ(nullptr, fc.current());

    //不能接收焦点
    v1->setFocusable(false);
    fc.setFocus(true);
    EXPECT_EQ(nullptr, fc.current());

    v1->setFocusable(true);
    fc.shift(v1);
    v1->setFocusable(false);
    fc.shiftIfNecessary();
    EXPECT_EQ(nullptr, fc.current());

    v1->setFocusable(true);
    fc.shift(v1);
    fc.removed(v1);
    EXPECT_EQ(nullptr, fc.current());
    //不能设置root为焦点
    fc.shift(root);
    EXPECT_EQ(nullptr, fc.current());

    v1->release();
    root->release();
}

class FocusControllerUnitTestBase : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        root = new Root;

        v1 = new View;
        v2 = new View;
        v3 = new View;
        v3->setFocusable(false);

        v4 = new View;

        v11 = new View;
        v12 = new View;
        v111 = new View;
        v111->setFocusable(false);
        v31 = new View;
        v41 = new View;
        v42 = new View;
        v41->setFocusable(false);

        root->attachChildToBack(v1);
        root->attachChildToBack(v2);
        root->attachChildToBack(v3);
        root->attachChildToBack(v4);
        v1->attachChildToBack(v11);
        v1->attachChildToBack(v12);
        v11->attachChildToBack(v111);
        v3->attachChildToBack(v31);
        v4->attachChildToBack(v41);
        v4->attachChildToBack(v42);
    }

    virtual void TearDown() override
    {
        root->recursiveDetachChildren();
        v31->release();
        v41->release();
        v42->release();

        v11->release();
        v12->release();
        v111->release();

        v1->release();
        v2->release();
        v3->release();
        v4->release();

        root->release();
    }
protected:
    Root * root;

    View * v1;
    View * v2;
    View * v3;
    View * v4;

    View * v11;
    View * v12;
    View * v111;
    View * v31;
    View * v41;
    View * v42;
};

TEST_F(FocusControllerUnitTestBase, KeyTabShift)
{
    KeyState state;
    state.state = 0;
    KeyEvent tab(kET_KEY_DOWN, root, kVKEY_TAB, state, false, 0);
    FocusController fc(root);
    EXPECT_EQ(nullptr, fc.current());
    EXPECT_FALSE(fc.handleKeyEvent(tab));
    EXPECT_EQ(nullptr, fc.current());

    fc.setFocus(true);
    fc.shift(v2);
    EXPECT_EQ(v2, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v31, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v4, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v42, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v1, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v11, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v12, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v2, fc.current());
}

TEST_F(FocusControllerUnitTestBase, KeyShiftTabShift)
{
    KeyState state;
    state.state = 0;
    KeyEvent shift_tab(kET_KEY_DOWN, root, kVKEY_TAB, state, false, kEF_SHIFT_DOWN);
    FocusController fc(root);
    EXPECT_EQ(nullptr, fc.current());
    EXPECT_FALSE(fc.handleKeyEvent(shift_tab));
    EXPECT_EQ(nullptr, fc.current());

    fc.setFocus(true);
    fc.shift(v2);
    EXPECT_EQ(v2, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v12, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v11, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v1, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v42, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v4, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v31, fc.current());
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v2, fc.current());
}

TEST_F(FocusControllerUnitTestBase, KeyShiftUnexpected)
{
    KeyState state;
    state.state = 0;
    KeyEvent shift_tab(kET_KEY_DOWN, root, kVKEY_TAB, state, false, kEF_SHIFT_DOWN);
    KeyEvent tab(kET_KEY_DOWN, root, kVKEY_TAB, state, false, 0);
    FocusController fc(root);
    EXPECT_EQ(nullptr, fc.current());
    EXPECT_FALSE(fc.handleKeyEvent(shift_tab));
    EXPECT_EQ(nullptr, fc.current());

    fc.setFocus(true);
    fc.shift(v2);
    EXPECT_EQ(v2, fc.current());

    //处理意外情况   
    fc.shift(v4);
    //current实际上不能接收焦点 
    v4->setFocusable(false);
    fc.handleKeyEvent(shift_tab);

    EXPECT_EQ(v31, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v42, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v1, fc.current());
    v4->setFocusable(true);

    //当前焦点已经从root上摘除 那么焦点从头开始
    fc.shift(v4);
    EXPECT_EQ(v4, fc.current());
    root->detachChild(v4);
    fc.handleKeyEvent(shift_tab);
    EXPECT_EQ(v31, fc.current());

    fc.handleKeyEvent(tab);
    EXPECT_EQ(v1, fc.current());
    fc.handleKeyEvent(tab);
    EXPECT_EQ(v11, fc.current());
}

}