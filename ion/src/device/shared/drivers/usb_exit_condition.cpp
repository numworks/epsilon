#include <ion/events.h>
#include <shared/drivers/usb.h>
#include <kernel/drivers/keyboard.h>

namespace Ion {
namespace Device {
namespace USB {

// On N100 this function is called directly via the in-RAM relocated DFU
Events::Event shouldInterruptDFU() {
  /* WARNING: we can't use hardware interruptions are their code is within the
   * kernel which might be being rewritten when USB is active...
   * Using Keyboard::scan here would be too slow, a mere
   * Ion::Timing::usleep(1000) is preventing the DFU device from being detected
   * and scan waits 100us for each row. */
  Keyboard::Key interruptKeys[] = {Keyboard::Key::Back, Keyboard::Key::Home, Keyboard::Key::OnOff};
  for (Keyboard::Key key : interruptKeys) {
    Keyboard::activateRow(Keyboard::rowForKey(key));
    if (Keyboard::columnIsActive(Keyboard::columnForKey(key))) {
      return Events::Event::PlainKey(key);
    }
  }
  return Events::None;
}

}
}
}
