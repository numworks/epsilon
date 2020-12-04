#ifndef ION_DEVICE_SHARED_SVCALL_ARGS_H
#define ION_DEVICE_SHARED_SVCALL_ARGS_H


/* According to arm documentation, available at :
 * https://developer.arm.com/documentation/ihi0042/latest/
 *  Typically, the registers r4-r8, r10 and r11 (v1-v5, v7 and v8) are used to
 *  hold the values of a routine’s local variables. Of these, only v1-v4 can be
 *  used uniformly by the whole Thumb instruction set, but the AAPCS does not
 *  require that Thumb code only use those registers.
 *  A subroutine must preserve the contents of the registers r4-r8, r10, r11
 *  and SP (and r9 in PCS variants that designate r9 as v6).
 * r10 and r8 seem to less likely to be used if a register is required between
 * storage and loading of an argument (which should be avoided anyway).
 * TODO HUGO : Extensively test using registers between STORE and LOAD.
 */

#define SVC_ARGS_REGISTER_0 "r10"
#define SVC_ARGS_REGISTER_1 "r8"

#define SVC_STORE_ARG(reg, argv) asm volatile ("mov " reg ",%0"::"r"(argv));
#define SVC_LOAD_ARG(reg, argv) asm volatile ("mov %0," reg :"=r"(argv):);


void svcall(unsigned int svcNumber, int argc = 0, void * argv[] = nullptr);

void getSvcallArgs(int argc, void * argv[]);

#endif