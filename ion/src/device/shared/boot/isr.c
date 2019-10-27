#include "isr.h"
extern const void * _stack_start;

/* Interrupt Service Routines are void->void functions */
typedef void(*ISR)(void);

/* Notice: The Cortex-M4 expects all jumps to be made at an odd address when
 * jumping to Thumb code. For example, if you want to execute Thumb code at
 * address 0x100, you'll have to jump to 0x101. Luckily, this idiosyncrasy is
 * properly handled by the C compiler that will generate proper addresses when
 * using function pointers. */

#define INITIALISATION_VECTOR_SIZE 0x71

ISR InitialisationVector[INITIALISATION_VECTOR_SIZE]
  __attribute__((section(".isr_vector_table")))
  __attribute__((used))
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
  isr_systick, // SysTick service routine
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
  0, // DMA1Stream6 service routine
  0, // ADC1 global interrupt
  0, // CAN1 TX interrupt
  0, // CAN1 RX0 interrupt
  0, // CAN1 RX1 interrupt
  0, // CAN1 SCE interrupt
  0, // EXTI Line[9:5] interrupts
  0, // TIM1 Break interrupt and TIM9 global interrupt
  0, // TIM1 update interrupt and TIM10 global interrupt
  0, // TIM1 Trigger & Commutation interrupts and TIM11 global interrupt
  0, // TIM1 Capture Compare interrupt
  0, // TIM2 global interrupt
  0, // TIM3 global interrupt
  0, // TIM4 global interrupt
  0, // I2C1 global event interrupt
  0, // I2C1 global error interrupt
  0, // I2C2 global event interrupt
  0, // I2C2 global error interrupt
  0, // SPI1 global interrupt
  0, // SPI2 global interrupt
  0, // USART1 global interrupt
  0, // USART2 global interrupt
  0, // USART3 global interrupt
  0, // EXTI Line[15:10] interrupts
  0, // EXTI Line 17 interrupt RTC Alarms (A and B) through EXTI line interrupt
  0, // EXTI Line 18 interrupt / USB On-The-Go FS Wakeup through EXTI line interrupt
  0, // TIM8 Break interrupt TIM12 global interrupt
  0, // TIM8 Update interrupt TIM13 global interrupt
  0, // TIM8 Trigger & Commutation interrupt TIM14 global interrupt
  0, // TIM8 Cap/Com interrupt
  0, // DMA1 global interrupt Channel 7
  0, // FSMC global interrupt
  0, // SDIO global interrupt
  0, // TIM5 global interrupt
  0, // SPI3 global interrupt
  0, // ?
  0, // ?
  0, // TIM6 global interrupt
  0, // TIM7 global interrupt
  0, // DMA2 Stream0 global interrupt
  0, // DMA2 Stream1 global interrupt
  0, // DMA2 Stream2 global interrupt
  0, // DMA2 Stream3 global interrupt
  0, // DMA2 Stream4 global interrupt
  0, // SD filter0 global interrupt
  0, // SD filter1 global interrupt
  0, // CAN2 TX interrupt
  0, // BXCAN2 RX0 interrupt
  0, // BXCAN2 RX1 interrupt
  0, // CAN2 SCE interrupt
  0, // USB On The Go FS global interrupt
  0, // DMA2 Stream5 global interrupts
  0, // DMA2 Stream6 global interrupt
  0, // DMA2 Stream7 global interrupt
  0, // USART6 global interrupt
  0, // I2C3 event interrupt
  0, // I2C3 error interrupt
  0, // ?
  0, // ?
  0, // ?
  0, // ?
  0, // ?
  0, // ?
  0, // RNG global interrupt
  0, // FPU global interrupt
  0, // ?
  0, // ?
  0, // SPI4 global interrupt
  0, // SPI5 global interrupt
  0, // ?
  0, // ?
  0, // ?
  0, // ?
  0, // ?
  0, // ?
  0, // Quad-SPI global interrupt
  0, // ?
  0, // ?
  0, // I2CFMP1 event interrupt
  0  // I2CFMP1 error interrupt
};
