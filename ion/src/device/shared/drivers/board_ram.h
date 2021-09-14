#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_RAM_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_RAM_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t writableSRAMStartAddress();
uint32_t writableSRAMEndAddress();

}
}
}

#endif
