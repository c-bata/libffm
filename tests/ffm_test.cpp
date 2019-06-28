#include <stdint.h>

#include "gtest/gtest.h"

#include "ffm.h"

using namespace ffm;

class FFMWeightReaderTest : public ::testing::Test {
protected:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

TEST_F(FFMWeightReaderTest, Read) {
  // TODO(c-bata): Avoid using magic relative path.
  ffm::WeightReader wfr("../../tests/weights_file1.txt");
  ffm::ffm_float actual = wfr.read(1);
  ASSERT_EQ(actual, 1.5);
}
