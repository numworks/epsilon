#include "calculator.h"
#include "../regs/regs.h"
#include "../keyboard.h"
#include <ion/usb.h>

namespace Ion {
namespace USB {
namespace Device {

void Calculator::Poll() {
  Calculator c;

  // Leave DFU mode when pressing the Back key.
  Ion::Keyboard::Key exitKey = Ion::Keyboard::Key::A6;
  uint8_t exitKeyRow = Ion::Keyboard::Device::rowForKey(exitKey);
  uint8_t exitKeyColumn = Ion::Keyboard::Device::columnForKey(exitKey);

  Ion::Keyboard::Device::activateRow(exitKeyRow);

  // TODO also leave on detach
  while (!(Ion::Keyboard::Device::columnIsActive(exitKeyColumn)) && Ion::USB::isPlugged()) {
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
