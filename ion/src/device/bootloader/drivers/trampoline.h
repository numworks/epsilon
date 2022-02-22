#ifndef ION_DEVICE_BOOTLOADER_DRIVERS_TRAMPOLINE_H
#define ION_DEVICE_BOOTLOADER_DRIVERS_TRAMPOLINE_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Trampoline {

constexpr int Suspend = 0;
constexpr int ExternalFlashEraseSector = 1;
constexpr int ExternalFlashWriteMemory = 2;

// TODO: Use the other available trampolines instead of liba's functions

uint32_t address(int index);

}
}
}

#endif
