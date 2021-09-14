#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_RAM_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_RAM_H

#include <stdint.h>

namespace Ion {
namespace Device {
namespace Board {

uint32_t writableSRAMStartAdress();
uint32_t writableSRAMEndAdress();

}
}
}

#endif
