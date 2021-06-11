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

void initInterruptions();
void shutdownInterruptions();

void initPeripherals(bool fromBootloader);
void shutdownPeripherals(bool keepLEDAwake = false);

void initInterruptionPriorities();
void setDefaultGPIO();

enum class Frequency {
  Low = 0,
  High = 1
};
void setStandardFrequency(Frequency f);

uint32_t slotAUserlandStart();
uint32_t slotBUserlandStart();
bool isInReflashableSector(uint32_t address);
uint32_t userlandStart();
void switchExecutableSlot(uint32_t address);
void downgradeTrustLevel(bool displayPopup);

bool addressInUserlandRAM(void * address);

}
}
}

#endif

