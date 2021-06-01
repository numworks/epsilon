#include <stdint.h>
#include <stdlib.h>

// __stack_chk_guard value initialization is done at platform initialization
__attribute__((used,section(".canary"))) uintptr_t __stack_chk_guard;

__attribute__((noreturn,used)) void __stack_chk_fail() {
  abort();
}
