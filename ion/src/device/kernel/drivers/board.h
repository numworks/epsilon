#ifndef ION_DEVICE_KERNEL_DRIVERS_BOARD_H
#define ION_DEVICE_KERNEL_DRIVERS_BOARD_H

#include <stdint.h>
#include <shared/drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

void initSystemClocks();
void initPeripheralsClocks();
void shutdownPeripheralsClocks(bool keepLEDAwake = false);

void initPeripherals(bool authentication, bool fromBootloader);
void shutdownPeripherals(bool keepLEDAwake = false);

enum class InterruptionPriority : uint8_t {
  High = 0,
  MediumHigh = 10,
  Medium = 20,
  MediumLow = 30,
  Low = 40
};

void initInterruptionPriorities();
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

