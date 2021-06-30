#ifndef ION_DEVICE_FLASHER_BOARD_H
#define ION_DEVICE_FLASHER_BOARD_H

#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

void initFPU();
void initMPU();

void initCompensationCell();
void shutdownCompensationCell();
void initPeripherals();
void shutdownPeripherals();

void initSystemClocks(); // n0110/shared/drivers/board.cpp
void initPeripheralsClocks();
void shutdownPeripheralsClocks();

void setDefaultGPIO();

}
}
}

#endif
