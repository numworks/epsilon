#include "keyboard.h"

// Public Ion::Keyboard methods

namespace Ion {
namespace Keyboard {

State scan() {
  /*
   * We don't share the same keyboard layout than with the NumWorks calculator,
   * just implement the back key here.
   */
  return Ion::Keyboard::Device::scan() & ((uint64_t)1 << 52) ? (State)(1 << (int)Ion::Keyboard::Key::A6) : (State)0;
}

}
}

// Private Ion::Keyboard::Device methods

namespace Ion {
namespace Keyboard {
namespace Device {

void init() {
  GPIO.GPDDAT()->setBitRange(8, 0, 0);
  for (uint8_t i=0; i<8; i++) {
    GPIO.GPDUDP()->setMode(i, GPIO::GPDUDP::Mode::Disabled);
    GPIO.GPDCON()->setMode(i, GPIO::GPDCON::Mode::Output);
  }

  for (uint8_t i=0; i<8; i++) {
    GPIO.GPDUDP()->setMode(i, GPIO::GPDUDP::Mode::PullDown);
    GPIO.GPGCON()->setMode(i, GPIO::GPGCON::Mode::Input);
  }
}

uint64_t scan() {
  uint64_t state = 0;
  for (int i = 0; i < 8; i++) {
    GPIO.GPDDAT()->setBitRange(8, 0, 1 << i);

    Ion::usleep(10);

    state |= (uint64_t)(GPIO.GPGDAT()->getBitRange(8, 0)) << (8*i);
  }
  GPIO.GPDDAT()->setBitRange(8, 0, 0);
  return state & 0x7E7E7E7E7E7EFEFF;
}

}
}
}
