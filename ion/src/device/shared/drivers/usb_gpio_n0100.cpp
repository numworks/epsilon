#include <drivers/usb.h>

namespace Ion {
namespace Device {
namespace USB {

void initGPIO() {
  // DEBUG GPIO pin
  GPIOC.MODER()->setMode(11, GPIO::MODER::Mode::Output);
  GPIOC.ODR()->set(11, false);

  /* Configure the GPIO */
  /* The Vbus pin is connected to the USB Vbus port. Depending on the
   * hardware, it should either be configured as an AlternateFunction, or as a
   * floating Input. */
  Config::VbusPin.init();
  Config::DmPin.init();
  Config::DpPin.init();
}

void shutdownGPIO() {
  constexpr static AFGPIOPin Pins[] = { Config::DpPin, Config::DmPin, Config::VbusPin };
  for (const AFGPIOPin & p : Pins) {
    p.shutdown();
  }
}

}
}
}
