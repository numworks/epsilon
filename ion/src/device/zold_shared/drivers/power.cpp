#include <ion/keyboard.h>
#include <drivers/board.h>
#include <drivers/external_flash.h>
#include <drivers/keyboard.h>
#include <drivers/power.h>
#include <drivers/reset.h>
#include <regs/regs.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>

namespace Ion {
namespace Device {
namespace Power {

// Public Power methods
using namespace Device::Regs;

void waitUntilOnOffKeyReleased() {
  /* Wait until power is released to avoid restarting just after suspending */
  bool isPowerDown = true;
  while (isPowerDown) {
    Keyboard::State scan = Keyboard::scan();
    isPowerDown = scan.keyDown(Keyboard::Key::OnOff);
  }
  Timing::msleep(100);
}


}
}
}
