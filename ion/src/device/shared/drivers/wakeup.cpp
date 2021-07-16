#include "wakeup.h"
#include <drivers/usb_privileged.h>
#include <drivers/config/usb.h>

namespace Ion {
namespace Device {
namespace WakeUp {

using namespace Regs;

void onUSBPlugging() {
  USB::initGPIO();
  /* Here, EXTICR3 register is filled between position 4-7 (Vbus pin = 9) with
   * 0000 (Vbus GPIO = group A). */
  SYSCFG.EXTICR()->setEXTI(USB::Config::VbusPin.pin(), USB::Config::VbusPin.group());

  EXTI.EMR()->set(USB::Config::VbusPin.pin(), true);
  EXTI.FTSR()->set(USB::Config::VbusPin.pin(), true);
  EXTI.RTSR()->set(USB::Config::VbusPin.pin(), true);
}

}
}
}
