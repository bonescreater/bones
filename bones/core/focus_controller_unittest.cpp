
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

}