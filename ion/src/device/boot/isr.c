#include "rt0.h"
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
  (ISR)&_stack_start, // Stack start
  start, // Reset service routine,
  0, // NMI service routine,
  abort, // HardFault service routine,
  0, // MemManage service routine,
  0, // BusFault service routine,
  0, // UsageFault service routine,
  0, 0, 0, 0, // Reserved
  0, // SVCall service routine,
  0, // DebugMonitor service routine,
  0, // Reserved
  0, // PendSV service routine,
  0, // SysTick service routine
  0, // WWDG service routine
  0, // PVD service routine
  0, // TampStamp service routine
  0, // RtcWakeup service routine
  0, // Flash service routine
  0, // RCC service routine
  0, // EXTI0 service routine
  0, // EXTI1 service routine
  0, // EXTI2 service routine
  0, // EXTI3 service routine
  0, // EXTI4 service routine
  0, // DMA1Stream0 service routine
  0, // DMA1Stream1 service routine
  0, // DMA1Stream2 service routine
  0, // DMA1Stream3 service routine
  0, // DMA1Stream4 service routine
  0, // DMA1Stream5 service routine
  0  // DMA1Stream6 service routine
};
