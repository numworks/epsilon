#include <main.h>
#include <stdint.h>
#include <string.h>
#include <boot/isr.h>
#include <boot/rt0.h>
#include <bootloader/drivers/board.h>
#include <bootloader/drivers/usb.h>
#include <shared/drivers/config/board.h>
#include <drivers/led.h>
#include <drivers/option_bytes.h>
#include <drivers/random.h>
#include <drivers/reset.h>
#include <ion/timing.h>

extern "C" {
  void abort();
  extern char _canary_section_start_ram;
  extern char _canary_section_end_ram;
  extern char _data_section_start_ram;
  extern char _data_section_end_ram;
  extern char _bss_section_start_ram;
  extern char _bss_section_end_ram;
  extern char _stack_end;
}

void __attribute__((noinline)) abort() {
#ifdef NDEBUG
  Ion::Device::USB::DFU();
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
 * We can safely assume that neither memcpy, memset, nor any Ion::Device::init*
 * method will use floating-point numbers, but bootloader_main very well can.
 * To make sure ion_main's potential usage of VFP registers doesn't bubble-up to
 * start(), we isolate it in its very own non-inlined function call.
 */

static void __attribute__((noinline)) jump_to_main() {
  bootloader_main();
}

/* When 'start' is executed, the external flash is supposed to be shutdown. We
 * thus forbid inlining to prevent executing this code from external flash
 * (just in case 'start' was to be called from the external flash). */

void __attribute__((noinline)) start() {
  /* This is where execution starts after reset.
   * Many things are not initialized yet so the code here has to pay attention.
    */

  /* Initialize the FPU as early as possible.
   * For example, static C++ objects are very likely to manipulate float values */
  Ion::Device::Board::initFPU();

  /* Because of the trampoline functions, the bootloader code can still be used
   * after booting. We assert the emptiness of the data/bss section of the
   * bootloader since we don't keep any RAM space for it. */
  assert(&_bss_section_end_ram == &_bss_section_start_ram);
  assert(&_data_section_end_ram == &_data_section_start_ram);

  Ion::Device::Init::configureGlobalVariables();
  Ion::Device::Board::init();

  /* Randomize stack position in bootloader
   * No variable is stored on the stack at this point and 'start' is the root
   * of the backtrace so we can change the stack start without losing any
   * information.
   * The minimal stack size is experimentally determined at 8K. We keep 16K
   * just in case. */
  static constexpr uint32_t k_minimalStackSize = 0x400;
  /* We compute the random range knowing that RAMSize = 256k = 0x40000,
   * minimalStackSize = 16k = 0x4000, the start_sart is aligned on 8 bytes; */
  uint32_t randomRange = Ion::Device::Board::Config::SRAMLength - k_minimalStackSize - 0x8;
  uint32_t randomOffset = (static_cast<uint64_t>(Ion::Device::random())*static_cast<uint64_t>(randomRange))/0x100000000;
  uint32_t newStackStart = reinterpret_cast<uint32_t>(&_stack_end) + k_minimalStackSize + randomOffset;
  asm volatile (
      "msr MSP, %[stackPointer]"
      : :
      [stackPointer] "r" (newStackStart)
    );

  /* The only variable of data we need is the canary (stack gard) which is
   * linked in its own eponymous section. We initialized it with a random value
   * which will change at each booting. The canary variable will be linked at
   * the same location in the kernel to ensure its avaibility within the
   * trampoline functions. The kernel won't initialize it again. The canary
   * initialization has to be done once the clocks are enabled in order to get
   * a hardware random number. */
  uint32_t randomToken = Ion::Device::random();
  memcpy(&_canary_section_start_ram, &randomToken, sizeof(randomToken));

  jump_to_main();

  abort();
}
