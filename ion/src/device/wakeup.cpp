#include "wakeup.h"
#include "regs/regs.h"
#include "battery.h"
#include "usb.h"
#include "keyboard.h"

namespace Ion {
namespace WakeUp {
namespace Device {

void onChargingEvent() {
  Battery::Device::initGPIO();

  /* Warning: pins with the same number in different groups cannot be set as
   * source input for EXTI at the same time. Here, EXTICR1 register is filled
   * between position 0-3 (charging pin = 0) with
   * 0000 (ChargingGPIO = group A). */
  SYSCFG.EXTICR1()->setEXTI(Battery::Device::ChargingPin, Battery::Device::ChargingGPIO);

  EXTI.EMR()->set(Battery::Device::ChargingPin, true);

  /* We need to detect when the battery stops charging. We set the
   * wake up event on the rising edge. */
  EXTI.RTSR()->set(Battery::Device::ChargingPin, true);
}

void onUSBPlugging() {
  USB::Device::initGPIO();
  /* Here, EXTICR3 register is filled between position 4-7 (Vbus pin = 9) with
   * 0000 (Vbus GPIO = group A). */
  SYSCFG.EXTICR3()->setEXTI(USB::Device::VbusPin.pin(), USB::Device::VbusPin.group());

  EXTI.EMR()->set(USB::Device::VbusPin.pin(), true);
#if LED_WHILE_CHARGING
  EXTI.FTSR()->set(USB::Device::VbusPin.pin(), true);
#endif
  EXTI.RTSR()->set(USB::Device::VbusPin.pin(), true);
}


void onPowerKeyDown() {
  Keyboard::Key key = Keyboard::Key::B2;
  uint8_t rowPin = Keyboard::Device::RowPins[Keyboard::Device::rowForKey(key)];
  Keyboard::Device::RowGPIO.MODER()->setMode(rowPin, GPIO::MODER::Mode::Output);
  Keyboard::Device::RowGPIO.OTYPER()->setType(rowPin, GPIO::OTYPER::Type::OpenDrain);
  Keyboard::Device::RowGPIO.ODR()->set(rowPin, 0);

  uint8_t column = Keyboard::Device::columnForKey(key);
  uint8_t columnPin = Keyboard::Device::ColumnPins[column];

  Keyboard::Device::ColumnGPIO.MODER()->setMode(columnPin, GPIO::MODER::Mode::Input);
  Keyboard::Device::ColumnGPIO.PUPDR()->setPull(columnPin, GPIO::PUPDR::Pull::Up);

  /* Here, EXTICR1 register is filled between position 4-7 (column pin = 1) with
   * 0010 (ColumnGPIO = group C). */

  SYSCFG.EXTICR1()->setEXTI(columnPin, Keyboard::Device::ColumnGPIO);

  EXTI.EMR()->set(columnPin, true);

  /* When the key is pressed, it will go from 1 (because it's pulled up) to
   * zero (because it's connected to the open-drain output. In other words,
   * we're waiting for a falling edge. */
  EXTI.FTSR()->set(columnPin, true);
}

}
}
}
