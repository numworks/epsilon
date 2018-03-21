#include "calculator.h"
#include "../regs/regs.h"
#include <ion/keyboard.h>

namespace Ion {
namespace USB {
namespace Device {

void Calculator::Poll() {
  // Wait for speed enumeration done
  while (!OTG.GINTSTS()->getENUMDNE()) {
  }

  Calculator c;
  // Leave DFU mode when pressing the Back key.
  // TODO also leave on detach or when unplugging
  while (!Ion::Keyboard::scan().keyDown(Ion::Keyboard::Key::A6)) {
    c.poll();
  }
}

Descriptor * Calculator::descriptor(uint8_t type, uint8_t index) {
  int typeCount = 0;
  for (size_t i=0; i<sizeof(m_descriptors)/sizeof(m_descriptors[0]); i++) {
    Descriptor * descriptor = m_descriptors[i];
    if (descriptor->type() != type) {
      continue;
    }
    if (typeCount == index) {
      return descriptor;
    } else {
      typeCount++;
    }
  }
  return nullptr;
}

}
}
}
