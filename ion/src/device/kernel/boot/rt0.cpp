#include "isr.h"
#include <stdint.h>
#include <string.h>
#include <boot/rt0.h>
#include <drivers/reset.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/config/board.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/timing.h>
#include <main.h>
#include <regs/regs.h>

void __attribute__((noinline)) abort() {
#ifdef NDEBUG
  Ion::Device::Reset::core();
#else
  while (1) {
  }
#endif
}

/* In order to ensure that this method is execute from the external flash, we
 * forbid inlining it.*/

static void __attribute__((noinline)) external_flash_start() {
  /* Init the peripherals. We do not initialize the backlight in case there is
   * an on boarding app: indeed, we don't want the user to see the LCD tests
   * happening during the on boarding app. The backlight will be initialized
   * after the Power-On Self-Test if there is one or before switching to the
   * home app otherwise. */
  bool numworksAuthentication = Ion::Device::Authentication::isAuthenticated(reinterpret_cast<void *>(Ion::Device::Board::Config::UserlandAddress - Ion::Device::Board::Config::SizeSize));
  Ion::Device::Board::initPeripherals(numworksAuthentication, true);
  return kernel_main(numworksAuthentication);
}

/* This additional function call 'jump_to_external_flash' serves two purposes:
 * - By default, the compiler is free to inline any function call he wants. If
 *   the compiler decides to inline some functions that make use of VFP
 *   registers, it will need to push VFP them onto the stack in calling
 *   function's prologue.
 *   Problem: in start()'s prologue, we would never had a chance to enable the
 *   FPU since this function is the first thing called after reset.
 *   We can safely assume that neither memcpy, memset, nor any Ion::Device::init*
 *   method will use floating-point numbers, but ion_main very well can.
 *   To make sure ion_main's potential usage of VFP registers doesn't bubble-up to
 *   start(), we isolate it in its very own non-inlined function call.
 * - To avoid jumping on the external flash when it is shut down, we ensure
 *   there is no symbol references from the internal flash to the external
 *   flash except this jump. In order to do that, we isolate this
 *   jump in a symbol that we link in a special section separated from the
 *   internal flash section. We can than forbid cross references from the
 *   internal flash to the external flash. */

static void __attribute__((noinline)) jump_to_external_flash() {
  external_flash_start();
}

/* When 'start' is executed, the external flash is supposed to be shutdown. We
 * thus forbid inlining to prevent executing this code from external flash
 * (just in case 'start' was to be called from the external flash). */

void __attribute__((noinline)) start() {
  /* This is where execution starts after reset.
   * Many things are not initialized yet so the code here has to pay attention. */

  Ion::Device::Init::configureRAM();
  Ion::Device::Board::init();

  /* At this point, we initialized clocks and the external flash but no other
   * peripherals. */

  jump_to_external_flash();

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
