#include <drivers/power.h>
#include <drivers/keyboard.h>

namespace Ion {
namespace Device {
namespace Power {

void waitUntilOnOffKeyReleased() {
  /* Wait until power is released to avoid restarting just after suspending */
  bool isPowerDown = true;
  while (isPowerDown) {
    Keyboard::State scan = Keyboard::scan();
    isPowerDown = scan.keyDown(Keyboard::Key::OnOff);
  }
}

}
}
}
