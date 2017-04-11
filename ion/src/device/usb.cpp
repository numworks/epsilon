#include <ion/usb.h>
#include "usb.h"
#include "regs/regs.h"

namespace Ion {
namespace USB {

bool isPlugged() {
  return Device::VbusPin.group().IDR()->get(Device::VbusPin.pin());
}

}
}

namespace Ion {
namespace USB {
namespace Device {

void init() {
  initGPIO();
}

void initGPIO() {
  /* Configure the GPIO
   * The VBUS pin is connected to the USB VBUS port. To read if the USB is
   * plugged, the pin must be pulled down. */
  VbusPin.group().MODER()->setMode(VbusPin.pin(), GPIO::MODER::Mode::Input);
  VbusPin.group().PUPDR()->setPull(VbusPin.pin(), GPIO::PUPDR::Pull::Down);
}

void shutdown() {
  VbusPin.group().MODER()->setMode(VbusPin.pin(), GPIO::MODER::Mode::Analog);
  VbusPin.group().PUPDR()->setPull(VbusPin.pin(), GPIO::PUPDR::Pull::None);
}

void generateWakeUpEventForUSBPlug() {
  initGPIO();
  /* Warning: pins with the same number in different groups cannot be set as
   * source input for EXTI at the same time. Here, EXTICR3 register is
   * filled between position 4-7 (Vbus pin = 9) with 0000 (Vbus GPIO = group A). */
  SYSCFG.EXTICR3()->setEXTI(VbusPin.pin(), VbusPin.group());

  EXTI.EMR()->set(VbusPin.pin(), true);
  EXTI.FTSR()->set(VbusPin.pin(), true);
  EXTI.RTSR()->set(VbusPin.pin(), true);
}
}
}
}
