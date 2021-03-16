#include <shared/drivers/usb.h>
#include <kernel/drivers/keyboard.h>

namespace Ion {
namespace Device {
namespace USB {

void initInterrupter() {
  // TODO: interrupt on 'home', 'power'
  // TODO: try to use Keyboard::scan? Is it too slow?
  /* WARNING: we can't use hardware interruptions are their code is within the
   * kernel which might be being rewritter when USB is active... */

  /* Leave DFU mode if the Back key is pressed, the calculator unplugged or the
   * USB core soft-disconnected. */
  Keyboard::activateRow(Keyboard::rowForKey(Ion::Keyboard::Key::Back));
}

bool shouldInterrupt() {
  return Keyboard::columnIsActive(Keyboard::columnForKey(Ion::Keyboard::Key::Back));
}

}
}
}
