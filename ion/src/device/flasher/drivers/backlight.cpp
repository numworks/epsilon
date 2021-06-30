#include "backlight.h"

namespace Ion {
namespace Device {
namespace Backlight {

void init() {
  initGPIO();
  sendPulses(16);
}

}
}
}

