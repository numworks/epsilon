#ifndef ION_DEVICE_SHARED_KEYBOARD_H
#define ION_DEVICE_SHARED_KEYBOARD_H

#include <ion/keyboard.h>
#include <ion/timing.h>
#include <drivers/config/keyboard.h>

namespace Ion {
namespace Device {
namespace Keyboard {

using namespace Ion::Keyboard;

void initGPIO();
void shutdownGPIO();
void initExtendedInterruptions();
bool shutdownExtendedInterruptions();
void stopPollTimer();

State scan();

inline void setRowState(uint16_t rowState) {
  // TODO: Assert pin numbers are sequentials and dynamically find 9 and 0
  Config::RowGPIO.ODR()->setBitRange(8, 0, rowState);

  // TODO: 100 us seems to work, but wasn't really calculated
  Ion::Timing::usleep(100);
}

inline void activateRow(uint8_t row) {
  /* In open-drain mode, a 0 in the register drives the pin low, and a 1 lets
   * the pin floating (Hi-Z). So we want to set the current row to zero and all
   * the others to 1. */
  uint16_t rowState = ~(1 << static_cast<uint16_t>(Config::RowPins[row]));
  setRowState(rowState);
}

inline void activateAllRows() {
  setRowState(0);
}

}
}
}

#endif
