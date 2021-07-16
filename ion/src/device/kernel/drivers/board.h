#ifndef ION_DEVICE_KERNEL_DRIVERS_BOARD_H
#define ION_DEVICE_KERNEL_DRIVERS_BOARD_H

#include <stdint.h>
#include <shared/drivers/board_privileged.h>
#include <shared/drivers/board_unprivileged.h>

namespace Ion {
namespace Device {
namespace Board {

void initSystemClocks();
void initPeripheralsClocks();
void shutdownPeripheralsClocks(bool keepLEDAwake = false);

void initInterruptions();
void shutdownInterruptions();

void initPeripherals(bool fromBootloader);
void shutdownPeripherals(bool keepLEDAwake = false);

void initInterruptionPriorities();
void setDefaultGPIO();

bool isInReflashableSector(uint32_t address);
void switchExecutableSlot(uint32_t address);
void enableExternalApps();

bool addressInUserlandRAM(void * address);

}
}
}

#endif

