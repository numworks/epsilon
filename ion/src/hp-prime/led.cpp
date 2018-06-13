#include <ion/led.h>
#include "led.h"
#include "regs/regs.h"

// Public Ion::LED methods

namespace Ion {
namespace LED {

void setColor(KDColor c) {
  GPIO.GPCDAT()->setBitRange(7, 7, c.red() > 0x7f);
  GPIO.GPCDAT()->setBitRange(6, 6, c.green() > 0x7f);
  GPIO.GPCDAT()->setBitRange(5, 5, c.blue() > 0x7f);
}

}
}

// Private Ion::Device::LED methods

namespace Ion {
namespace LED {
namespace Device {

void init() {
  GPIO.GPCCON()->setMode(5, GPIO::GPCCON::Mode::Output);
  GPIO.GPCCON()->setMode(6, GPIO::GPCCON::Mode::Output);
  GPIO.GPCCON()->setMode(7, GPIO::GPCCON::Mode::Output);
  Ion::LED::setColor(KDColorBlack);
}

}
}
}
