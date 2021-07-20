#include "board_frequency.h"
#include <drivers/board.h>

namespace Ion {
namespace Device {
namespace Board {

Frequency sStandardFrequency = Frequency::High;

void setStandardFrequency(Frequency f) {
  sStandardFrequency = f;
  setClockStandardFrequency();
}

}
}
}
