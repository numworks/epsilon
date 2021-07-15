#include "isr.h"
#include <stdint.h>
#include <string.h>
#include <boot/rt0.h>
#include <drivers/reset.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/timing.h>
#include <main.h>
#include <regs/regs.h>

extern "C" {
  extern char _isr_vector_table_start_flash;
  extern char _isr_vector_table_start_ram;
  extern char _isr_vector_table_end_ram;
}

void __attribute__((noinline)) abort() {
#ifdef NDEBUG
  Ion::Device::Reset::core();
#else
  while (1) {
  }
#endif
}

void __attribute__((noinline)) jump_to_main() {
  Ion::Device::Board::initPeripherals(true);
  return kernel_main();
}

/* When 'start' is executed, the external flash is supposed to be shutdown. We
 * thus forbid inlining to prevent executing this code from external flash
 * (just in case 'start' was to be called from the external flash). */

void __attribute__((noinline)) start() {
  /* This is where execution starts after reset.
   * Many things are not initialized yet so the code here has to pay attention. */

  Ion::Device::Init::configureRAM();

  /* Copy isr_vector_table section to RAM
   * The isr table must be within the memory mapped by the microcontroller (it
   * can't live in the external flash). */
  size_t isrSectionLength = (&_isr_vector_table_end_ram - &_isr_vector_table_start_ram);
  memcpy(&_isr_vector_table_start_ram, &_isr_vector_table_start_flash, isrSectionLength);


  Ion::Device::Board::init();

  /* By default, the compiler is free to inline any function call he wants. If
   * the compiler decides to inline some functions that make use of VFP registers,
   * it will need to push VFP them onto the stack in calling function's prologue.
   * Problem: in start()'s prologue, we would never had a chance to enable the
   * FPU since this function is the first thing called after reset.
   * We can safely assume that neither memcpy, memset, nor any Ion::Device::init*
   * method will use floating-point numbers, but bootloader_main very well can.
   * To make sure ion_main's potential usage of VFP registers doesn't bubble-up to
   * start(), we isolate it in its very own non-inlined function call
   */

  jump_to_main();

  abort();
}

void __attribute__((interrupt, noinline)) isr_systick() {
  auto t = Ion::Device::Timing::MillisElapsed;
  t++;
  Ion::Device::Timing::MillisElapsed = t;
}

void __attribute__((interrupt, noinline)) keyboard_handler() {
  Ion::Device::Keyboard::handleInterruption();
}

void __attribute__((interrupt, noinline)) tim2_handler() {
  Ion::Device::Events::stall();
}

void __attribute__((interrupt, noinline)) tim4_handler() {
  Ion::Device::Keyboard::debounce();
}
