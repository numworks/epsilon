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

void _start();

ISR InitialisationVector[INITIALISATION_VECTOR_SIZE]
  __attribute__((section(".isr_vector_table")))
  = {
  (ISR)&_stack_start,
  _ResetServiceRoutine,
  _NMIServiceRoutine,
  _HardFaultServiceRoutine,
  _MemManageServiceRoutine,
  _BusFaultServiceRoutine,
  _UsageFaultServiceRoutine,
  0, 0, 0, 0, // Reserved
  _SVCallServiceRoutine,
  _DebugMonitorServiceRoutine,
  0, // Reserved
  _PendSVServiceRoutine,
  _SysTickServiceRoutine,
  _WWDGServiceRoutine,
  _PVDServiceRoutine,
  _TampStampServiceRoutine,
  _RtcWakeupServiceRoutine,
  _FlashServiceRoutine,
  _RCCServiceRoutine,
  _EXTI0ServiceRoutine,
  _EXTI1ServiceRoutine,
  _EXTI2ServiceRoutine,
  _EXTI3ServiceRoutine,
  _EXTI4ServiceRoutine,
  _DMA1Stream0ServiceRoutine,
  _DMA1Stream1ServiceRoutine,
  _DMA1Stream2ServiceRoutine,
  _DMA1Stream3ServiceRoutine,
  _DMA1Stream4ServiceRoutine,
  _DMA1Stream5ServiceRoutine,
  _DMA1Stream6ServiceRoutine
};
