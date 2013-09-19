#include <stdio.h>
#include <assert.h>

#include <jit.h>

// accepts two cmd arguments:
// - source file, string
// - function name, string
// function prototype should be (int*)()
int main(int argc, char** argv)
{
  if (argc < 3)
  {
    printf("Not enough arguments passed. ");
    printf("Usage: jit.nexe <file> <function>\n");
    return 1;
  }

  // get function compiled, retreive pointer
  void* ptr = getFunctionPointer(argv[1], argv[2]);
  assert (ptr != 0);
  typedef int (*function)();
  function fptr = (function)ptr;
  // do the actual job
  int res = fptr();
  // clear up after llvm
  shutdown();
  return res;
}

