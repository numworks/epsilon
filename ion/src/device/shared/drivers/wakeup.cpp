#include "wakeup.h"
#include <drivers/battery.h>
#include <drivers/keyboard.h>
#include <drivers/usb.h>
#include <drivers/config/battery.h>
#include <drivers/config/keyboard.h>
#include <drivers/config/usb.h>
#include <regs/regs.h>

namespace Ion {
namespace Device {
namespace WakeUp {

using namespace Regs;

void onChargingEvent() {
  Battery::initGPIO();

  /* Warning: pins with the same number in different groups cannot be set as
   * source input for EXTI at the same time. Here, EXTICR1 register is filled
   * between:
   * - N0100: position 0-3 (charging pin = 0) with  0000 (ChargingGPIO = group A)
   * - N0110: position 12-15 (charging pin = 3) with 0100 (ChargingGPIO = group E)
   */
  SYSCFG.EXTICR1()->setEXTI(Battery::Config::ChargingPin.pin(), Battery::Config::ChargingPin.group());

  EXTI.EMR()->set(Battery::Config::ChargingPin.pin(), true);

  /* We need to detect when the battery stops charging. We set the
   * wake up event on the rising edge. */
  EXTI.RTSR()->set(Battery::Config::ChargingPin.pin(), true);
}

void onUSBPlugging() {
  USB::initGPIO();
  /* Here, EXTICR3 register is filled between position 4-7 (Vbus pin = 9) with
   * 0000 (Vbus GPIO = group A). */
  SYSCFG.EXTICR3()->setEXTI(USB::Config::VbusPin.pin(), USB::Config::VbusPin.group());

  EXTI.EMR()->set(USB::Config::VbusPin.pin(), true);
  EXTI.FTSR()->set(USB::Config::VbusPin.pin(), true);
  EXTI.RTSR()->set(USB::Config::VbusPin.pin(), true);
}


void onOnOffKeyDown() {
  Keyboard::Key key = Keyboard::Key::OnOff;
  uint8_t rowPin = Keyboard::Config::RowPins[Keyboard::rowForKey(key)];
  Keyboard::Config::RowGPIO.MODER()->setMode(rowPin, GPIO::MODER::Mode::Output);
  Keyboard::Config::RowGPIO.OTYPER()->setType(rowPin, GPIO::OTYPER::Type::OpenDrain);
  Keyboard::Config::RowGPIO.ODR()->set(rowPin, 0);

  uint8_t column = Keyboard::columnForKey(key);
  uint8_t columnPin = Keyboard::Config::ColumnPins[column];

  Keyboard::Config::ColumnGPIO.MODER()->setMode(columnPin, GPIO::MODER::Mode::Input);
  Keyboard::Config::ColumnGPIO.PUPDR()->setPull(columnPin, GPIO::PUPDR::Pull::Up);

  /* Here, EXTICR1 register is filled between:
   * - N0100: position 4-7 (column pin = 1) with 0010 (ColumnGPIO = group C)
   * - N0110: position 8-11 (column pin = 2) with 0010 (ColumnGPIO = group C). */

  SYSCFG.EXTICR1()->setEXTI(columnPin, Keyboard::Config::ColumnGPIO);

  EXTI.EMR()->set(columnPin, true);

  /* When the key is pressed, it will go from 1 (because it's pulled up) to
   * zero (because it's connected to the open-drain output. In other words,
   * we're waiting for a falling edge. */
  EXTI.FTSR()->set(columnPin, true);
}

}
}
}
