#include "gtest/gtest.h"

TEST(libjitTest, NonExistentFileTest) {
  FAIL();
}

TEST(libjitTest, ExistentFileTest) {
  FAIL();
}

TEST(libjitTest, NonExistentFunctionTest) {
  FAIL();
}

TEST(libjitTest, ExistentFunctionTest) {
  FAIL();
}

TEST(libjitTest, FunctionRunTest) {
  FAIL();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
