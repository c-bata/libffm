#include <stdint.h>

#include "gtest/gtest.h"

#include "ffm.h"

using namespace ffm;

class FFMWeightReaderTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

TEST_F(FFMWeightReaderTest, Read)
{
    ffm::ffm_load_model("./foo.txt");
    ffm::WeightReader wfr("foo.txt");
    ffm::ffm_float actual = wfr.read(1);
    ASSERT_EQ(actual, 1.0);
    wfr.close();
}
