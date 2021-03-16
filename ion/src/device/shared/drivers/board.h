#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_H

#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

void init();
void shutdown();

void initInterruptions();
void shutdownInterruptions();

void setClockLowFrequency();
void setClockStandardFrequency();

}
}
}

#endif

