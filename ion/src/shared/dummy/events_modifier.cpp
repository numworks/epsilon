#include <ion/keyboard.h>

namespace Ion {
namespace Keyboard {

ShiftAlphaStatus shiftAlphaStatus() {
  return ShiftAlphaStatus::Default;
}

void setShiftAlphaStatus(ShiftAlphaStatus s) {
}

bool isShiftActive() {
  return false;
}

bool isAlphaActive() {
  return false;
}

void updateModifiersFromEvent(Event e) {
}

}
}
