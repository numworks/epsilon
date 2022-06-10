#include <config/board.h>
#include <drivers/board.h>
#include <drivers/cache.h>
#include <drivers/usb.h>
#include <ion/events.h>
#include <ion/usb.h>
#include <assert.h>
#include <string.h>

extern const void * _process_stack_end;
extern char _dfu_bootloader_flash_start;
extern char _dfu_bootloader_flash_end;

namespace Ion {
namespace USB {

typedef Events::Event (*PollFunctionPointer)();

Events::Event DFU() {
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
  char * dfu_bootloader_ram_start = reinterpret_cast<char *>(&_process_stack_end);
  assert(&_process_stack_end == (void *)(Device::Board::Config::UserlandSRAMOrigin + Device::Board::Config::UserlandSRAMLength - Device::Board::Config::UserlandStackLength));

  /* 2- Verify there is enough free space on the stack to copy the DFU code. */
  char foo;
  char * stackPointer = &foo;
  if (dfu_bootloader_ram_start + dfu_bootloader_size > stackPointer) {
    // There is not enough room on the stack to copy the DFU bootloader.
    return Events::None;
  }

  /* 3- Copy the DFU bootloader from Flash to RAM. */

  memcpy(dfu_bootloader_ram_start, &_dfu_bootloader_flash_start, dfu_bootloader_size);
  /* The DFU bootloader might have been copied in the DCache. However, when we
   * run the instructions from the DFU bootloader, the CPU looks for
   * instructions in the ICache and then in the RAM. We thus need to flush the
   * DCache to update the RAM. */
  // Flush data cache
  /* TODO: dfu_relocated is build only for N0100 model which has no Cache.
   * Clean the building system in order to get rid of the comment. */
  // Device::Cache::cleanDCache();

  Device::USB::willExecuteDFU();

  /* 4- Jump to DFU bootloader code. We made sure in the linker script that the
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

  Events::Event abortReason = dfu_bootloader_entry();

  Device::USB::didExecuteDFU();

  /* 5- That's all. The DFU bootloader on the stack is now dead code that will
   * be overwritten when the stack grows. */
  return abortReason;
}

}
}
