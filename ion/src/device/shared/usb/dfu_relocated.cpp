#include <ion.h>
#include <ion/usb.h>
#include <string.h>
#include <assert.h>
#include <drivers/cache.h>
#include "../drivers/timing.h"

extern const void * _stack_end;
extern char _dfu_bootloader_flash_start;
extern char _dfu_bootloader_flash_end;

namespace Ion {
namespace USB {

typedef void (*PollFunctionPointer)(bool exitWithKeyboard, void * data);

void DFU(bool exitWithKeyboard, void * data) {
  Ion::updateSlotInfo();

  /* DFU transfers can serve two purposes:
   *  - Transfering RAM data between the machine and a host, e.g. Python scripts
   *  - Upgrading the flash memory to perform a software update
   *
   * The second case raises a huge issue: code cannot be executed from memory
   * that is being modified. We're solving this issue by copying the DFU code in
   * RAM.
   *
   * The new DFU address in RAM needs to be temporarily overwriteable when the
   * program is being run. Epsilon has a large stack to allow deeply recursive
   * code to run, but when doing DFU transfers it is safe to assume we will need
   * very little stack space. We're therefore using the topmost 8K of the stack
   * reserved by Epsilon. */

  /* 1- The stack being in reverse order, the end of the stack will be the
   * beginning of the DFU bootloader copied in RAM. */

  size_t dfu_bootloader_size = &_dfu_bootloader_flash_end - &_dfu_bootloader_flash_start;
  char * dfu_bootloader_ram_start = reinterpret_cast<char *>(&_stack_end);
  assert(&_stack_end == (void *)(0x20000000 + 256*1024 - 32*1024));

  /* 2- Verify there is enough free space on the stack to copy the DFU code. */

  char foo;
  char * stackPointer = &foo;
  if (dfu_bootloader_ram_start + dfu_bootloader_size > stackPointer) {
    // There is not enough room on the stack to copy the DFU bootloader.
    return;
  }

  /* 3- Copy the DFU bootloader from Flash to RAM. */

  memcpy(dfu_bootloader_ram_start, &_dfu_bootloader_flash_start, dfu_bootloader_size);
  /* The DFU bootloader might have been copied in the DCache. However, when we
   * run the instructions from the DFU bootloader, the CPU looks for
   * instructions in the ICache and then in the RAM. We thus need to flush the
   * DCache to update the RAM. */
  // Flush data cache
  Device::Cache::cleanDCache();

  /* 4- Disable all interrupts
   * The interrupt service routines live in the Flash and could be overwritten
   * by garbage during a firmware upgrade operation, so we disable them. */
  Device::Timing::shutdown();

  /* 5- Jump to DFU bootloader code. We made sure in the linker script that the
   * first function we want to call is at the beginning of the DFU code. */

  PollFunctionPointer dfu_bootloader_entry = reinterpret_cast<PollFunctionPointer>(dfu_bootloader_ram_start);

  /* To have the right debug symbols for the reallocated code, break here and:
   *  - Get the address of the new .text section
   *        In a terminal: arm-none-eabi-readelf -a ion/src/device/usb/dfu.elf
   *  - Delete the current symbol table
   *        symbol-file
   *  - Add the new symbol table, with the address of the new .text section
   *        add-symbol-file ion/src/device/usb/dfu.elf 0x20038000
   */

  dfu_bootloader_entry(exitWithKeyboard, data);

  /* 5- Restore interrupts */
  Device::Timing::init();

  /* 6- That's all. The DFU bootloader on the stack is now dead code that will
   * be overwritten when the stack grows. */
}

}
}
