#ifndef ION_DEVICE_N0100_CONFIG_EXTERNAL_FLASH_H
#define ION_DEVICE_N0100_CONFIG_EXTERNAL_FLASH_H

#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {
namespace Config {

using namespace Regs;

constexpr static uint32_t StartAddress = 0xFFFFFFFF;
constexpr static uint32_t EndAddress = 0xFFFFFFFF;
constexpr static int NumberOf4KSectors = 0;
constexpr static int NumberOf32KSectors = 0;
constexpr static int NumberOf64KSectors = 0;
constexpr static int NumberOfSectors = NumberOf4KSectors + NumberOf32KSectors + NumberOf64KSectors;
constexpr static AFGPIOPin Pins[] = {};

}
}
}
}

#endif
