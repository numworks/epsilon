#include "svcall.h"

/* According to arm documentation, available at :
 * https://developer.arm.com/documentation/ihi0042/latest/
 *  Typically, the registers r4-r8, r10 and r11 (v1-v5, v7 and v8) are used to
 *  hold the values of a routine’s local variables. Of these, only v1-v4 can be
 *  used uniformly by the whole Thumb instruction set, but the AAPCS does not
 *  require that Thumb code only use those registers.
 *  A subroutine must preserve the contents of the registers r4-r8, r10, r11
 *  and SP (and r9 in PCS variants that designate r9 as v6).
 * r10 and r8 are less likely to be used by other routines.
 * To preserve the contents of the registers, their content is stored on the
 * Stack and put back after using svc().
 * TODO Hugo : Consider a "typical" ARM subroutine calling convention :
 * - In the prologue, push r4 to r11 to the stack, and push the return address
 *   in r14 to the stack (this can be done with a single STM instruction);
 * - Copy any passed arguments (in r0 to r3) to the local scratch registers (r4
 *   to r11);
 * - Allocate other local variables to the remaining local scratch registers (r4
 *   to r11);
 * - Do calculations and call other subroutines as necessary using BL, assuming
 *   r0 to r3, r12 and r14 will not be preserved;
 * - Put the result in r0;
 * - In the epilogue, pull r4 to r11 from the stack, and pull the return address
 *   to the program counter r15. This can be done with a single LDM instruction.
 */

#define SVC_ARGS_REGISTER_0 "r10"
#define SVC_ARGS_REGISTER_1 "r8"

#define SVC_STORE_ARG(reg, argv) asm volatile ("mov " reg ",%0"::"r"(argv));
#define SVC_LOAD_ARG(reg, argv) asm volatile ("mov %0," reg :"=r"(argv):);

void getSvcallArgs(int argc, void * argv[]) {
  if (argc > 0) {
    SVC_LOAD_ARG(SVC_ARGS_REGISTER_0, argv[0]);
    if (argc > 1) {
      SVC_LOAD_ARG(SVC_ARGS_REGISTER_1, argv[1]);
    }
  }
}

void setSvcallArgs(int argc, void * argv[]) {
  if (argc > 0) {
    SVC_STORE_ARG(SVC_ARGS_REGISTER_0, argv[0]);
    if (argc > 1) {
      SVC_STORE_ARG(SVC_ARGS_REGISTER_1, argv[1]);
    }
  }
}

void svcall(unsigned int svcNumber, int argc, void * argv[]) {
  // Save previous registers state
  void * regState[2];
  getSvcallArgs(argc, regState);
  // Write registers
  setSvcallArgs(argc, argv);
  /* TODO Hugo : The following line triggers a compilation error :
   * svc(svcNumber); -> asm operand 0 probably doesn't match constraints
   * Current fix is to switch on SVC calls */
  switch (svcNumber) {
    case SVC_PUSH_RECT:
      svc(SVC_PUSH_RECT);
      break;
    case SVC_PUSH_RECT_UNIFORM:
      svc(SVC_PUSH_RECT_UNIFORM);
      break;
    case SVC_PULL_RECT:
      svc(SVC_PULL_RECT);
      break;
    default:
      break;
  }
  // Restore registers state
  setSvcallArgs(argc, regState);
}
