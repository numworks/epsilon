#ifndef ION_DEVICE_KERNEL_DRIVERS_BOARD_H
#define ION_DEVICE_KERNEL_DRIVERS_BOARD_H

#include <stdint.h>
#include <shared/drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

void initPeripheralsClocks();
void shutdownPeripheralsClocks(bool keepLEDAwake = false);

void initPeripherals(bool authentication, bool fromBootloader);
void shutdownPeripherals(bool keepLEDAwake = false);

void initInterruptions();
void shutdownInterruptions();
void setDefaultGPIO();

enum class Frequency {
  Low = 0,
  High = 1
};
void setStandardFrequency(Frequency f);

}
}
}

#endif

