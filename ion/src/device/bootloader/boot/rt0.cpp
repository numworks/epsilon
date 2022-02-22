#include <stdint.h>
#include <string.h>
#include <ion.h>
#include <boot/isr.h>
#include <drivers/board.h>
#include <drivers/rtc.h>
#include <drivers/reset.h>
#include <drivers/timing.h>

typedef void (*cxx_constructor)();

extern "C" {
  extern char _data_section_start_flash;
  extern char _data_section_start_ram;
  extern char _data_section_end_ram;
  extern char _bss_section_start_ram;
  extern char _bss_section_end_ram;
  extern cxx_constructor _init_array_start;
  extern cxx_constructor _init_array_end;

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

/* In order to ensure that this method is execute from the external flash, we
 * forbid inlining it.*/

static void __attribute__((noinline)) external_flash_start() {
  /* Init the peripherals. We do not initialize the backlight in case there is
   * an on boarding app: indeed, we don't want the user to see the LCD tests
   * happening during the on boarding app. The backlight will be initialized
   * after the Power-On Self-Test if there is one or before switching to the
   * home app otherwise. */
  Ion::Device::Board::initPeripherals(false);

  return ion_main(0, nullptr);
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

  /* Initialize the FPU as early as possible.
   * For example, static C++ objects are very likely to manipulate float values */
  Ion::Device::Board::initFPU();

  /* Copy data section to RAM
   * The data section is R/W but its initialization value matters. It's stored
   * in Flash, but linked as if it were in RAM. Now's our opportunity to copy
   * it. Note that until then the data section (e.g. global variables) contains
   * garbage values and should not be used. */
  size_t dataSectionLength = (&_data_section_end_ram - &_data_section_start_ram);
  memcpy(&_data_section_start_ram, &_data_section_start_flash, dataSectionLength);

  /* Zero-out the bss section in RAM
   * Until we do, any uninitialized global variable will be unusable. */
  size_t bssSectionLength = (&_bss_section_end_ram - &_bss_section_start_ram);
  memset(&_bss_section_start_ram, 0, bssSectionLength);

  /* Call static C++ object constructors
   * The C++ compiler creates an initialization function for each static object.
   * The linker then stores the address of each of those functions consecutively
   * between _init_array_start and _init_array_end. So to initialize all C++
   * static objects we just have to iterate between theses two addresses and
   * call the pointed function. */
#define SUPPORT_CPP_GLOBAL_CONSTRUCTORS 0
#if SUPPORT_CPP_GLOBAL_CONSTRUCTORS
  for (cxx_constructor * c = &_init_array_start; c<&_init_array_end; c++) {
    (*c)();
  }
#else
  /* In practice, static initialized objects are a terrible idea. Since the init
   * order is not specified, most often than not this yields the dreaded static
   * init order fiasco. How about bypassing the issue altogether? */
  if (&_init_array_start != &_init_array_end) {
    abort();
  }
#endif

  /* Copy isr_vector_table section to RAM
   * The isr table must be within the memory mapped by the microcontroller (it
   * can't live in the external flash). */
  size_t isrSectionLength = (&_isr_vector_table_end_ram - &_isr_vector_table_start_ram);
  memcpy(&_isr_vector_table_start_ram, &_isr_vector_table_start_flash, isrSectionLength);

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
