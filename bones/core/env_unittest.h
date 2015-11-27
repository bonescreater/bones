
#include "gtest\gtest.h"
#include "core_imp.h"

namespace bones
{

class ENVCORE : public testing::Test
{
protected:
    virtual void SetUp() override
    {
        Core::Config cf = { nullptr, Log::kNone, true, false, nullptr };
        Core::StartUp(cf);
    }
    virtual void TearDown() override
    {
        Core::ShutDown();
    }
};
}