#include "main.h"
#include <boot/isr.h>
#include <boot/rt0.h>
#include <drivers/board.h>
#include <drivers/reset.h>

extern "C" {
  void abort();
}

void __attribute__((noinline)) abort() {
#ifdef NDEBUG
  waitForInstruction();
#else
  while (1) {
  }
#endif
}

/* By default, the compiler is free to inline any function call he wants. If
 * the compiler decides to inline some functions that make use of VFP registers,
 * it will need to push VFP them onto the stack in calling function's prologue.
 * Problem: in start()'s prologue, we would never had a chance to enable the
 * FPU since this function is the first thing called after reset.
 */

static void __attribute__((noinline)) jump_to_main() {
  ion_main();
}

/* When 'start' is executed, the external flash is supposed to be shutdown. We
 * thus forbid inlining to prevent executing this code from external flash
 * (just in case 'start' was to be called from the external flash). */

void __attribute__((noinline)) start() {
  /* This is where execution starts after reset.
   * Many things are not initialized yet so the code here has to pay attention. */

  /* Initialize the FPU as early as possible.
   * For example, static C++ objects are very likely to manipulate float values. */
  Ion::Device::Board::initFPU();

  Ion::Device::Init::configureGlobalVariables();
  Ion::Device::Board::init();

  jump_to_main();

  abort();
}
