#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_H

#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

void initFPU();
void init();
void shutdown();
enum class Frequency {
  Low = 0,
  High = 1
};
void setFrequency(Frequency f);
}
}
}

#endif

