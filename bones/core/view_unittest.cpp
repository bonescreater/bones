#include "env_unittest.h"
#include "view.h"
#include "root.h"
#include "rect.h"
#include "point.h"
#include "size.h"

namespace bones
{

TEST(ViewUnitTest, CheckConstructor)
{
    View * v = new View;
    EXPECT_TRUE(v->flag_.focusable);
    EXPECT_TRUE(v->flag_.mouseable);
    EXPECT_TRUE(v->flag_.visible);
    EXPECT_EQ(0, v->children_count_);
    EXPECT_EQ(-1, v->group_id_);
    EXPECT_EQ(1.0f, v->opacity_);
    v->release();
}

TEST(ViewUnitTest, Completeness)
{
    View * v = new View;
    auto r = new Root;
    r->attachChildToBack(v);
    EXPECT_EQ(r, v->getRoot());

    v->setOpacity(0.5f);
    EXPECT_EQ(0.5f, v->getOpacity());

    v->setLoc(1, 2);
    EXPECT_EQ(1, v->getLoc().x());
    EXPECT_EQ(2, v->getLoc().y());
    EXPECT_EQ(1, v->getLeft());
    EXPECT_EQ(2, v->getTop());
    v->setSize(5, 6);
    EXPECT_EQ(5, v->getSize().width());
    EXPECT_EQ(6, v->getSize().height());
    EXPECT_EQ(5, v->getWidth());
    EXPECT_EQ(6, v->getHeight());
    Rect bounds;
    v->getLocalBounds(bounds);
    EXPECT_EQ(0, bounds.left());
    EXPECT_EQ(0, bounds.top());
    EXPECT_EQ(5, bounds.right());
    EXPECT_EQ(6, bounds.bottom());
    v->getGlobalBounds(bounds);
    EXPECT_EQ(1, bounds.left());
    EXPECT_EQ(2, bounds.top());
    EXPECT_EQ(6, bounds.right());
    EXPECT_EQ(8, bounds.bottom());
    
    EXPECT_EQ(6, v->mapToGlobal(Point::Make(5, 5)).x());
    EXPECT_EQ(7, v->mapToGlobal(Point::Make(5, 5)).y());
    EXPECT_EQ(4, v->mapToLocal(Point::Make(5, 5)).x());
    EXPECT_EQ(3, v->mapToLocal(Point::Make(5, 5)).y());

    v->setLoc(Point::Make(3, 4));
    EXPECT_EQ(3, v->getLeft());
    EXPECT_EQ(4, v->getTop());
    v->setSize(Size::Make(7, 8));
    EXPECT_EQ(7, v->getWidth());
    EXPECT_EQ(8, v->getHeight());
    v->setLeft(0);
    EXPECT_EQ(0, v->getLeft());
    v->setTop(0);
    EXPECT_EQ(0, v->getTop());
    v->setWidth(0);
    EXPECT_EQ(0, v->getWidth());
    v->setHeight(0);
    EXPECT_EQ(0, v->getHeight());


    v->setMouseable(false);
    EXPECT_FALSE(v->mouseable());
    v->setFocusable(false);
    EXPECT_FALSE(v->focusable());
    v->setVisible(false);
    EXPECT_FALSE(v->visible());

    v->setMouseable(true);
    EXPECT_TRUE(v->mouseable());
    EXPECT_FALSE(v->isMouseable());
    v->setFocusable(true);
    EXPECT_TRUE(v->focusable());
    EXPECT_FALSE(v->isFocusable());
    v->setVisible(true);
    EXPECT_TRUE(v->isFocusable());
    EXPECT_TRUE(v->isMouseable());

    r->recursiveDetachChildren();
    v->release();
    r->release();
}

class ViewUnitTestBase : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        v = new View;
        v1 = new View;
        v11 = new View;
        v12 = new View;
        v111 = new View;

        v2 = new View;
        v3 = new View;
        v31 = new View;
        v32 = new View;
        v321 = new View;
    }

    virtual void TearDown() override
    {
        v->recursiveDetachChildren();

        EXPECT_EQ(1, v->getRefCount());
        v->release();
        EXPECT_EQ(1, v1->getRefCount());
        v1->release();
        EXPECT_EQ(1, v11->getRefCount());
        v11->release();
        EXPECT_EQ(1, v12->getRefCount());
        v12->release();
        EXPECT_EQ(1, v111->getRefCount());
        v111->release();
        EXPECT_EQ(1, v2->getRefCount());
        v2->release();
        EXPECT_EQ(1, v3->getRefCount());
        v3->release();
        EXPECT_EQ(1, v31->getRefCount());
        v31->release();
        EXPECT_EQ(1, v32->getRefCount());
        v32->release();
        EXPECT_EQ(1, v321->getRefCount());
        v321->release();
    }
protected:
    View * v = new View;
    View * v1 = new View;
    View * v11 = new View;
    View * v12 = new View;
    View * v111 = new View;

    View * v2 = new View;
    View * v3 = new View;
    View * v31 = new View;
    View * v32 = new View;
    View * v321 = new View;
};

TEST_F(ViewUnitTestBase, RecursiveDetach)
{
    v->attachChildToBack(v1);
    v->attachChildToBack(v2);
    v->attachChildToBack(v3);

    v1->attachChildToBack(v11);
    v1->attachChildToBack(v12);
    v11->attachChildToBack(v111);
    v3->attachChildToBack(v31);
    v3->attachChildToBack(v32);
    v32->attachChildToBack(v321);
    v->recursiveDetachChildren();

    EXPECT_EQ(0, v->getChildCount());
    EXPECT_EQ(0, v1->getChildCount());
    EXPECT_EQ(0, v11->getChildCount());
    EXPECT_EQ(0, v3->getChildCount());
    EXPECT_EQ(0, v32->getChildCount());
}

TEST_F(ViewUnitTestBase, AttachBack)
{
    v->attachChildToBack(v1);
    v->attachChildToBack(v2);
    v->attachChildToBack(v3);
    EXPECT_EQ(3, v->getChildCount());
    EXPECT_EQ(v1, v->getFirstChild());
    EXPECT_EQ(v3, v->getLastChild());
    EXPECT_EQ(v2, v1->getNextSibling());
    EXPECT_EQ(v3, v2->getNextSibling());
    EXPECT_EQ(nullptr, v3->getNextSibling());
    EXPECT_EQ(v2, v3->getPrevSibling());
    EXPECT_EQ(v1, v2->getPrevSibling());
    EXPECT_EQ(nullptr, v1->getPrevSibling());
    EXPECT_EQ(v, v1->parent());
    EXPECT_EQ(v, v2->parent());
    EXPECT_EQ(v, v3->parent());

    EXPECT_EQ(v1, v->getChildAt(0));
    EXPECT_EQ(v2, v->getChildAt(1));
    EXPECT_EQ(nullptr, v->getChildAt(1000));
    //添加已经在子链表上的child 不变
    v->attachChildToBack(v1);
    EXPECT_EQ(3, v->getChildCount());
    EXPECT_EQ(v1, v->getFirstChild());
    EXPECT_EQ(v3, v->getLastChild());
    v->attachChildToBack(nullptr);
    EXPECT_EQ(3, v->getChildCount());
}

TEST_F(ViewUnitTestBase, DetachFromParent)
{
    v->attachChildToBack(v1);
    v->attachChildToBack(v2);
    v->attachChildToBack(v3);

    v2->detachFromParent();
    EXPECT_EQ(2, v->getChildCount());
    EXPECT_EQ(v1, v->getFirstChild());
    EXPECT_EQ(v3, v1->getNextSibling());
    v1->detachFromParent();
    EXPECT_EQ(v3, v->getFirstChild());
    v3->detachFromParent();
    EXPECT_EQ(nullptr, v->getFirstChild());
    EXPECT_EQ(nullptr, v->getLastChild());
}

TEST_F(ViewUnitTestBase, DetachChild)
{
    v->attachChildToBack(v1);
    v->attachChildToBack(v2);
    v->attachChildToBack(v3);

    v->detachChild(v3);
    EXPECT_EQ(v2, v->getLastChild());

    v->detachChildren();
    EXPECT_EQ(0, v->getChildCount());
    EXPECT_EQ(nullptr, v->getFirstChild());
    EXPECT_EQ(nullptr, v->getLastChild());
}

TEST_F(ViewUnitTestBase, AttachFront)
{
    v->attachChildToFront(v3);
    v->attachChildToFront(v2);
    v->attachChildToFront(v1);
    EXPECT_EQ(3, v->getChildCount());
    EXPECT_EQ(v1, v->getFirstChild());
    EXPECT_EQ(v3, v->getLastChild());
    EXPECT_EQ(v2, v1->getNextSibling());
    EXPECT_EQ(v3, v2->getNextSibling());
    EXPECT_EQ(nullptr, v3->getNextSibling());
    EXPECT_EQ(v2, v3->getPrevSibling());
    EXPECT_EQ(v1, v2->getPrevSibling());
    EXPECT_EQ(nullptr, v1->getPrevSibling());
    v->attachChildToFront(v3);
    EXPECT_EQ(3, v->getChildCount());
    EXPECT_EQ(v1, v->getFirstChild());
    EXPECT_EQ(v3, v->getLastChild());
    v->attachChildToFront(nullptr);
    EXPECT_EQ(3, v->getChildCount());
}

TEST_F(ViewUnitTestBase, AttachChildAt)
{
    v->attachChildAt(v1, 5);
    EXPECT_EQ(0, v->getChildCount());

    v->attachChildAt(v1, 0);
    v->attachChildAt(v3, 1);
    v->attachChildAt(v2, 1);
    EXPECT_EQ(3, v->getChildCount());
    EXPECT_EQ(v1, v->getFirstChild());
    EXPECT_EQ(v3, v->getLastChild());
    EXPECT_EQ(v2, v1->getNextSibling());
    EXPECT_EQ(v3, v2->getNextSibling());
    EXPECT_EQ(nullptr, v3->getNextSibling());
    EXPECT_EQ(v2, v3->getPrevSibling());
    EXPECT_EQ(v1, v2->getPrevSibling());
    EXPECT_EQ(nullptr, v1->getPrevSibling());
}

TEST_F(ViewUnitTestBase, Hittest)
{
    v->attachChildToBack(v1);
    v->attachChildToBack(v2);
    v->attachChildToBack(v3);

    v1->attachChildToBack(v11);
    v1->attachChildToBack(v12);
    v11->attachChildToBack(v111);
    v3->attachChildToBack(v31);
    v3->attachChildToBack(v32);
    v32->attachChildToBack(v321);
    v->setSize(20, 20);
    v1->setLoc(1, 1);
    v1->setSize(9, 9);
    v11->setSize(3, 3);
    v12->setLoc(2, 2);
    v12->setSize(4, 4);
    EXPECT_EQ(v12, v->hitTest(Point::Make(3, 3)));
    v12->setMouseable(false);
    EXPECT_EQ(v11, v->hitTest(Point::Make(3, 3)));
}




}