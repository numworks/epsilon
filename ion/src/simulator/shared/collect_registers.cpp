#include <ion.h>

extern "C" {

// define in assembly code
// Force the name as archs (linux/macos) don't mangle C names the same way
extern uintptr_t collect_registers(uintptr_t *regs) asm("_collect_registers");
}
namespace Ion {

// Wrapper to avoid handling c++ name mangling when writing assembly code

uintptr_t collectRegisters(jmp_buf buf) {
  uintptr_t *regs = (uintptr_t *)buf;
  return collect_registers(regs);
}

}  // namespace Ion
