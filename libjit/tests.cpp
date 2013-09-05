#include "gtest/gtest.h"
#include "jit.h"

const char ExistentFile[] = "/dev/src.cpp";
const char NonExistentFile[] = "___src___.cpp";
const char ExistentFunction[] = "function1";
const char NonExistentFunction[] = "___function1____";

TEST(libjitTest, NonExistentFileTest) {
  EXPECT_EQ(getFunctionPointer(NonExistentFile, ""), (void*)NULL);
}

TEST(libjitTest, ExistentFileTest) {
  EXPECT_NE(getFunctionPointer(ExistentFile, ExistentFunction), (void*)NULL);
  EXPECT_EQ(getFunctionPointer(ExistentFile, NonExistentFunction), (void*)NULL);
  EXPECT_NE(getFunctionPointer(ExistentFile, ExistentFunction), (void*)NULL);
}

TEST(libjitTest, NonExistentFunctionTest) {
  EXPECT_EQ(getFunctionPointer(ExistentFile, NonExistentFunction), (void*)NULL);
}

TEST(libjitTest, ExistentFunctionTest) {
  EXPECT_NE(getFunctionPointer(ExistentFile, ExistentFunction), (void*)NULL);
}

TEST(libjitTest, FunctionRunTest) {
  void* ptr = getFunctionPointer(ExistentFile, ExistentFunction);
  ASSERT_NE(ptr, (void*)NULL);
  typedef int (*fptr)(int, int);
  fptr function = (fptr) ptr;
  ASSERT_NE(function, (void*)NULL);
  // function = two int's sum
  EXPECT_EQ(function(0, 0), 0);
  EXPECT_EQ(function(1, 5), 6);
  EXPECT_EQ(function(5, -9), -4);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
