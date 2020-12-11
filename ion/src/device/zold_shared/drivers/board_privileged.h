#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_PRIVILEGED_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_PRIVILEGED_H

#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

void init();

void initFPU();
void initClocks();
void shutdownClocks(bool keepLEDAwake = false);

void initPeripherals(bool initBacklight);
void shutdownPeripherals(bool keepLEDAwake = false);

enum class Frequency {
  Low = 0,
  High = 1
};

void setStandardFrequency(Frequency f);
void setLowClockFrequencyHandler();
void setStandardClockFrequencyHandler();


}
}
}

#endif
