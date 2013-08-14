#include <dlfcn.h>



#include "gtest/gtest.h"

extern "C" {
int plus_seven(int a) {
  return a+7;
}

int five() {
  return 5;
}
}




TEST(zdlTest, BasicOpenCloseTest) {
  void *handle1 = dlopen(NULL, RTLD_NOW);
  EXPECT_NE(handle1, (void*)NULL);
  EXPECT_STREQ(dlerror(), "");
  EXPECT_EQ(dlclose(handle1), 0);

  void *handle2 = dlopen("test-file1.so", RTLD_NOW);
  EXPECT_EQ(handle2, (void*)NULL);
  EXPECT_STREQ(dlerror(), "Dynamic loading is not supported yet!");
  EXPECT_NE(dlclose(handle2), 0);


}

TEST(zdlTest, DLSymTest) {
  void *handle1 = dlopen(NULL, RTLD_NOW);

  EXPECT_NE(handle1, (void*)NULL);
  EXPECT_EQ(dlsym(handle1, ""), (void*)NULL);
  EXPECT_EQ(dlclose(handle1), 0);


  void *handle2 = dlopen("test-file1.so", RTLD_NOW);
  EXPECT_EQ(handle2, (void*)NULL);
  EXPECT_EQ(dlsym(handle2, ""), (void*)NULL);
  EXPECT_NE(dlclose(handle2), 0);
}

TEST(zdlTest, SymbolsLookupTest) {
  void *handle1 = dlopen(NULL, RTLD_NOW);
  EXPECT_NE(handle1, (void*)NULL);

  EXPECT_NE(dlsym(handle1, "plus_seven"), (void*)NULL);
  EXPECT_NE(dlsym(handle1, "five"), (void*)NULL);
  EXPECT_NE(dlsym(handle1, "printf"), (void*)NULL);
  EXPECT_EQ(dlsym(handle1, "____sdasd____"), (void*)NULL);

  EXPECT_EQ(dlclose(handle1), 0);
}

TEST(zdlTest, GLibcExtensionsTest) {
  Dl_info info;
  int res = dladdr((void*)plus_seven, &info);
  EXPECT_EQ(res, 0);
  EXPECT_EQ(info.dli_saddr, (char*)NULL);
  EXPECT_EQ(info.dli_sname, (char*)NULL);

  void *handle1 = dlopen(NULL, RTLD_NOW);
  EXPECT_NE(handle1, (void*)NULL);

  const char version[] = "2.0.0";
  EXPECT_EQ(dlsym(handle1, "plus_seven"), dlvsym(handle1, "plus_seven", version));
  EXPECT_EQ(dlsym(handle1, "five"), dlvsym(handle1, "five", version));
  EXPECT_EQ(dlsym(handle1, "printf"), dlvsym(handle1, "printf", version));

}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
