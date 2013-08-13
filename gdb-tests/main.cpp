#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zvm.h"


struct BStruct {
  int     a;
  double  b;
  bool    c;
  int     e[10];
};


class ATest {
public:
  ATest() {
  }
  ~ATest() {

  }

  virtual int value() {
    return 1;
  }

  void init() {
    a = 2;
    b = 3;
    c = 4.5;
    d = 7.8;
    strcpy(text, "Hello, GDB!\n");

    e.a = 20;
    e.b = 22.3;
    e.c = false;
    for (int i = 0; i < 10; ++i) {
      e.e[i] = 50 + i * 2;
    }
  }

  int       a;
  int       b;
  double    c;
  float     d;
  char      text[256];
  BStruct   e;

  void printMe(int count = 1);
  void printText(const char* str) ;
};

  void ATest::printMe(int count) {
    for (int i = 0; i < count; ++i)
      printf("%s @ %p\n", __PRETTY_FUNCTION__, this);
  }

  void ATest::printText(const char* str) {
    printf("%s @ %p\n", str, this);
  }

class BTest : public ATest {
public:
  virtual int value() {
    return 2;
  }
};

void print() {
  printf("Hello, world!\n");
}

int value(int c) {
  return c + 10;
}

ATest g; BTest g2;
int main(int argc, char** argv) {
  ATest t; BTest t2;
  ATest* ptr = new ATest(); ATest* ptr2 = new BTest();
    
  t.init(); t2.init();
  g.init(); g2.init();
  ptr->init(); ptr2->init();

  int sum = 0;
  for (int i=0;i<1000;++i)
    sum += i;









  const char buffer[] = "Hello, trusted!\n";
  // breakpoint here! do not move 'sum = 0' from line 100
  zvm_pwrite(1, buffer, sizeof(buffer), 0);
  //
  delete ptr; delete ptr2;
  return 0;
}
