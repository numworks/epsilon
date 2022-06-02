#include <ion/events.h>
#include <shared/drivers/usb.h>
#include <kernel/drivers/keyboard.h>

namespace Ion {
namespace Device {
namespace USB {

void scanKey(Keyboard::Key key, Events::Event * event) {
  if (*event == Events::None && Keyboard::columnIsActive(Keyboard::columnForKey(key))) {
    *event = Events::Event::PlainKey(key);
  }
}

// On N100 this function is called directly via the in-RAM relocated DFU
Events::Event shouldInterruptDFU() {
  /* WARNING: we can't use hardware interruptions are their code is within the
   * kernel which might be being rewritten when USB is active...
   * Using Keyboard::scan here would be too slow, a mere
   * Ion::Timing::usleep(1000) is preventing the DFU device from being detected
   * and scan waits 100us for each row. */
  Events::Event event = Events::None;
  Keyboard::activateRow(Keyboard::rowForKey(Keyboard::Key::Back));
  scanKey(Keyboard::Key::Back, &event);
  Keyboard::activateRow(Keyboard::rowForKey(Keyboard::Key::Home));
  scanKey(Keyboard::Key::Home, &event);
  // Avoid spending time re-activating the same row
  assert(Keyboard::rowForKey(Keyboard::Key::OnOff) == Keyboard::rowForKey(Keyboard::Key::Home));
  scanKey(Keyboard::Key::OnOff, &event);
  return event;
}

}
}
}
