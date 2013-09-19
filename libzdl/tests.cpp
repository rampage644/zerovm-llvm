#include <stdio.h>
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
  EXPECT_EQ(info.dli_saddr, (void*)NULL);
  EXPECT_EQ(info.dli_sname, (void*)NULL);

  void *handle1 = dlopen(NULL, RTLD_NOW);
  EXPECT_NE(handle1, (void*)NULL);

  const char version[] = "2.0.0";
  EXPECT_EQ(dlsym(handle1, "plus_seven"), dlvsym(handle1, "plus_seven", version));
  EXPECT_EQ(dlsym(handle1, "five"), dlvsym(handle1, "five", version));
  EXPECT_EQ(dlsym(handle1, "printf"), dlvsym(handle1, "printf", version));

  EXPECT_EQ(dlclose(handle1), 0);
}

TEST(zdlTest, MultipleOpenCloseTest) {
  void* handle1 = dlopen(NULL, RTLD_NOW);

  ASSERT_NE(handle1, (void*)NULL);
  EXPECT_STREQ(dlerror(), "");
  // five sym is defined in this object file
  EXPECT_NE(dlsym(handle1, "five"), (void*)NULL);
  EXPECT_STREQ(dlerror(), "");

  void* handle2 = dlopen(NULL, RTLD_NOW);

  ASSERT_NE(handle2, (void*)NULL);
  ASSERT_EQ(handle1, handle2);
  EXPECT_STREQ(dlerror(), "");
  // five sym is defined in this object file
  EXPECT_NE(dlsym(handle2, "five"), (void*)NULL);
  EXPECT_EQ(dlsym(handle2, "five"), dlsym(handle1, "five"));
  EXPECT_STREQ(dlerror(), "");

  // closing first handle, second should still work
  EXPECT_EQ(dlclose(handle1), 0);
  EXPECT_NE(dlsym(handle2, "five"), (void*)NULL);
  EXPECT_STREQ(dlerror(), "");

  // closing second handle, subsequent dlsym, dlclose should fail
  EXPECT_EQ(dlclose(handle2), 0);
  EXPECT_STREQ(dlerror(), "");

  // try to fail
  EXPECT_EQ(dlsym(handle2, "five"), (void*)NULL);
  EXPECT_STREQ(dlerror(), "Current handle is not opened.");
  EXPECT_EQ(dlclose(handle2), -1);
  EXPECT_STREQ(dlerror(), "Current handle is not opened.");

  // try to open again!
  handle1 = dlopen(NULL, RTLD_NOW);

  ASSERT_NE(handle1, (void*)NULL);
  EXPECT_STREQ(dlerror(), "");
  // five sym is defined in this object file
  EXPECT_NE(dlsym(handle1, "five"), (void*)NULL);
  EXPECT_STREQ(dlerror(), "");
  EXPECT_EQ(dlclose(handle1), 0);
}

TEST(zdlTest, MisUseTest) {
  // that'll be our handle
  int var = 0;

  ASSERT_EQ(dlsym((void*)&var, "five"), (void*)NULL);
  EXPECT_STREQ(dlerror(), "Current handle is not opened.");
  ASSERT_NE(dlclose((void*)&var), 0);
  EXPECT_STREQ(dlerror(), "Current handle is not opened.");
}

TEST(zdlTest, FunctionResolutionAndExecutionTest) {
  void* handle1 = dlopen(NULL, RTLD_NOW);
  ASSERT_NE(handle1, (void*)NULL);
  EXPECT_STREQ(dlerror(), "");

  void *ptr = dlsym(handle1, "islower");
  ASSERT_NE(ptr, (void*)NULL);
  typedef int (*islower_ptr)(int);
  EXPECT_EQ(((islower_ptr)ptr)('A'), 0);
  EXPECT_NE(((islower_ptr)ptr)('c'), 0);

  ptr = dlsym(handle1, "fopen");
  ASSERT_NE(ptr, (void*)NULL);

  ptr = dlsym(handle1, "malloc");
  ASSERT_NE(ptr, (void*)NULL);
  typedef void* (*malloc_ptr)(size_t);
  void* memory = ((malloc_ptr)ptr)(sizeof(int));
  *(int*)memory = 0xffffffff;
  EXPECT_NE(memory, (void*)0);

  ptr = dlsym(handle1, "free");
  ASSERT_NE(ptr, (void*)NULL);
  typedef void (*free_ptr)(void*);
  ((free_ptr)ptr)(memory);

  ptr = dlsym(handle1, "fstat");
  ASSERT_NE(ptr, (void*)NULL);

  ptr = dlsym(handle1, "rand");
  ASSERT_NE(ptr, (void*)NULL);
  typedef int (*rand_ptr)();
  EXPECT_EQ(dlclose(handle1), 0);
}

TEST(zdlTest, CPlusPlusFunctionsTest) {
  void* handle1 = dlopen(NULL, RTLD_NOW);
  ASSERT_NE(handle1, (void*)NULL);
  EXPECT_STREQ(dlerror(), "");

  // operator new
  void *ptr = dlsym(handle1, "_Znwj");
  ASSERT_NE(ptr, (void*)NULL);
  typedef void* (*new_ptr)(int);
  void* memory = ((new_ptr)ptr)(sizeof(int));
  EXPECT_NE(memory, (void*)0);
  *(int*)memory = 0xffffffff;

  // operator delete
  ptr = dlsym(handle1, "_ZdlPv");
  ASSERT_NE(ptr, (void*)NULL);
  typedef void (*free_ptr)(void*);
  ((free_ptr)ptr)(memory);


  EXPECT_EQ(dlclose(handle1), 0);
}

TEST(zdlTest, libcFunctionsResolutionTest) {
  return;
  void* handle1 = dlopen(NULL, RTLD_NOW);
  ASSERT_NE(handle1, (void*)NULL);
  EXPECT_STREQ(dlerror(), "");

  // open file with symbol names
  FILE* file = fopen("/dev/external_symbols_libc.a.txt", "r");
  char buffer[256];
  ASSERT_NE(file, (void*)NULL);
  void* ptr = 0;
  int counter = 0,
      failures = 0;

  while (fgets(buffer, sizeof(buffer), file)) {
    char *pos;
    if ((pos=strchr(buffer, '\n')) != NULL)
        *pos = '\0';

    ptr = (void*)dlsym(handle1, buffer);
    EXPECT_NE(ptr, (void*)NULL) << "Couldn't find " << buffer << " (" << dlerror() << ")";
    if (ptr == (void*)NULL)
      ++failures;
    ++counter;
  }
  printf("counter=%d, failures=%d\n", counter, failures);
  fclose(file);
  EXPECT_EQ(dlclose(handle1), 0);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
