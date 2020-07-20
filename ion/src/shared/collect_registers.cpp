#include <ion.h>
#include <setjmp.h>

namespace Ion {

/* Forbid inlining to ensure dummy to be at the top of the stack. Otherwise,
 * LTO inlining can make regs lower on the stack than some just-allocated
 * pointers. */
__attribute__((noinline))uintptr_t collectRegisters(jmp_buf buf) {
  /* TODO: we use setjmp to get the registers values to look for python heap
   * root. However, the 'setjmp' does not guarantee that it gets all registers
   * values. We should check our setjmp implementation for the device and
   * ensure that it also works for other platforms. */
  setjmp(buf);
  int dummy;
  return (uintptr_t)&dummy;
}

}
