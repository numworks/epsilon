#include <string.h>

#include <ion/src/device/shared/drivers/external_flash.h>
#include <ion/src/device/n0110/drivers/power.h>

#include <bootloader/trampoline.h>
#include <bootloader/boot.h>

namespace Bootloader {

void __attribute__((noinline)) suspend() {
  Ion::Device::Power::internalFlashSuspend(true);
}

void* Trampolines[TRAMPOLINES_COUNT]
  __attribute__((section(".trampolines_table")))
  __attribute__((used))
 = {
  (void*) Bootloader::suspend, // Suspend
  (void*) Ion::Device::ExternalFlash::EraseSector, // External erase
  (void*) Ion::Device::ExternalFlash::WriteMemory, // External write
  (void*) memcmp,
  (void*) memcpy,
  (void*) memmove,
  (void*) memset,
  (void*) strchr,
  (void*) strcmp,
  (void*) strlcat,
  (void*) strlcpy,
  (void*) strlen,
  (void*) strncmp
};

void* CustomTrampolines[CUSTOM_TRAMPOLINES_COUNT]
  __attribute__((section(".custom_trampolines_table")))
  __attribute__((used))
 = {
  (void*) Bootloader::Boot::mode,
  // This function doesn't do anything ...
  (void*) Bootloader::Boot::setMode
};

}
