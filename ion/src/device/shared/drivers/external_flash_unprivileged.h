#ifndef ION_DEVICE_SHARED_EXTERNAL_FLASH_UNPRIVILEGED_H
#define ION_DEVICE_SHARED_EXTERNAL_FLASH_UNPRIVILEGED_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

int SectorAtAddress(uint32_t address);

static constexpr uint8_t NumberOfAddressBitsIn64KbyteBlock = 16;
static constexpr uint8_t NumberOfAddressBitsIn32KbyteBlock = 15;
static constexpr uint8_t NumberOfAddressBitsIn4KbyteBlock = 12;

}
}
}

#endif
