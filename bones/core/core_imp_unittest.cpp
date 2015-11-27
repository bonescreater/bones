#include "env_unittest.h"
#include "core_imp.h"
#include "point.h"

namespace bones
{
TEST(CoreUnitTest, CheckConstructor)
{
    EXPECT_FALSE(Core::CEFEnable());
    EXPECT_FALSE(Core::AntiAliasEnable());
    EXPECT_EQ(nullptr, Core::GetAnimationManager());
    EXPECT_EQ(nullptr, Core::GetCSSManager());
    EXPECT_EQ(nullptr, Core::GetResManager());
    EXPECT_EQ(nullptr, Core::GetRootManager());
    EXPECT_EQ(nullptr, Core::GetXMLController());

    Core::Config cf = { nullptr, Log::kNone, true, false, nullptr };
    ASSERT_TRUE(Core::StartUp(cf));
    EXPECT_FALSE(Core::CEFEnable());
    EXPECT_TRUE(Core::AntiAliasEnable());
    EXPECT_NE(nullptr, Core::GetAnimationManager());
    EXPECT_NE(nullptr, Core::GetCSSManager());
    EXPECT_NE(nullptr, Core::GetResManager());
    EXPECT_NE(nullptr, Core::GetRootManager());
    EXPECT_NE(nullptr, Core::GetXMLController());

    Core::ShutDown();
}

TEST(CoreUnitTest, Completeness)
{
    Core::Config cf = { nullptr, Log::kNone, true, false, nullptr };
    ASSERT_TRUE(Core::StartUp(cf));
    Point begin = { 0, 0 };
    Point end = { 0, 1 };
    Color cr[] = { BONES_RGB_BLACK, BONES_RGB_WHITE };
    Scalar pos[] = { 0, 1 };
    auto linear = Core::createLinearGradient(begin, end, 2, cr, pos, Core::kClamp);
    EXPECT_NE(nullptr, linear);
    Core::destroyShader(linear);

    Point center = { 0, 0 };
    Scalar radius = 1;
    auto radial = Core::createRadialGradient(center, radius, 2, cr, pos, Core::kClamp);
    EXPECT_NE(nullptr, radial);
    Core::destroyShader(radial);

    //创建默认的dasheffect
    auto effect = Core::createDashEffect(0, 0, 0);
    EXPECT_NE(nullptr, effect);
    Core::destroyEffect(effect);
    Scalar interval[2] = { 10, 20 };
    effect = Core::createDashEffect(2, interval, 5);
    EXPECT_NE(nullptr, effect);
    Core::destroyEffect(effect);

    EXPECT_NE(nullptr, Core::getDashEffectCache());

    Core::ShutDown();
}

}