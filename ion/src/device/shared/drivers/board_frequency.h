#ifndef ION_DEVICE_SHARED_DRIVERS_BOARD_FREQUENCY_H
#define ION_DEVICE_SHARED_DRIVERS_BOARD_FREQUENCY_H

namespace Ion {
namespace Device {
namespace Board {

enum class Frequency {
  Low = 0,
  High = 1
};

void setStandardFrequency(Frequency f);

}
}
}

#endif
