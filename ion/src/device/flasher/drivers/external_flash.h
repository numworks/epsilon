#ifndef ION_DEVICE_FLASHER_EXTERNAL_FLASH_H
#define ION_DEVICE_FLASHER_EXTERNAL_FLASH_H

#include <shared/drivers/external_flash.h>

namespace Ion {
namespace Device {
namespace ExternalFlash {

void init();
void shutdown();
void JDECid(uint8_t * manufacturerID, uint8_t * memoryType, uint8_t * capacityType);


}
}
}

#endif
