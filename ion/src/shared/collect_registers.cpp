#include <ion.h>
#include <setjmp.h>

namespace Ion {

/* Forbid inlining to ensure dummy to be at the top of the stack. Otherwise,
 * LTO inlining can make regs lower on the stack than some just-allocated
 * pointers. */
__attribute__((noinline))uintptr_t collectRegisters(jmp_buf buf) {
  setjmp(buf);
  int dummy;
  return (uintptr_t)&dummy;
}

}
