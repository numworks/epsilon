#include "isr.h"

extern const void * _stack_start;

/* Interrupt Service Routines are void->void functions */
typedef void(*ISR)(void);

/* Notice: The Cortex-M4 expects all jumps to be made at an odd address when
 * jumping to Thumb code. For example, if you want to execute Thumb code at
 * address 0x100, you'll have to jump to 0x101. Luckily, this idiosyncrasy is
 * properly handled by the C compiler that will generate proper addresses when
 * using function pointers. */

#define INITIALISATION_VECTOR_SIZE 0x6B

ISR InitialisationVector[INITIALISATION_VECTOR_SIZE]
  __attribute__((section(".isr_vector_table")))
  = {
  (ISR)&_stack_start,
  _ResetServiceRoutine,
  _NMIServiceRoutine,
  _HardFaultServiceRoutine,
  _MemManageServiceRoutine,
  _BusFaultServiceRoutine,
  0, // UsageFault
  0, // Reserved
  _SVCallServiceRoutine,
  0, // Debug Monitor
  _PendSVServiceRoutine,
  _SysTickServiceRoutine,
  0
};
