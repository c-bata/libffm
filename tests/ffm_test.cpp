#include <stdint.h>
#include "gtest/gtest.h"

extern "C"
{
#include "ffm.h"
}

class FFMTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(FFMTest, Foo)
{
ASSERT_EQ(0, 0);
}
