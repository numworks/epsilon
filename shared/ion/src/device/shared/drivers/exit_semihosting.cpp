#include <omg/unreachable.h>
#include <stdlib.h>

extern "C" {
__attribute__((noreturn)) void exit(int code) {
  /* Use semihosting to warn qemu that the tests are finished using a dummy BKPT
   * https://developer.arm.com/documentation/dui0203/j/semihosting/semihosting-operations/angel-swireason-reportexception--0x18-
   */
  constexpr int ADP_Stopped_RunTimeErrorUnknown = 0x20023;
  constexpr int ADP_Stopped_ApplicationExit = 0x20026;
  register int r0 asm("r0") = 0x18;  // angel_SWIreason_ReportException
  register int r1 asm("r1") =
      code == 0 ? ADP_Stopped_ApplicationExit : ADP_Stopped_RunTimeErrorUnknown;
  asm volatile("bkpt 0xAB" : : "r"(r0), "r"(r1) : "memory");
  OMG_UNREACHABLE;
}
}
