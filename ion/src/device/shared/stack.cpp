#include <ion.h>

extern const void * _stack_start;
extern const void * _stack_end;

bool Ion::stackSafe() {
  volatile int stackDummy;
  volatile void * stackPointer = &stackDummy;
  return (stackPointer >= &_stack_end && stackPointer <= &_stack_start);
}
