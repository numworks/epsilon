#include <ion/events.h>

namespace Ion {
namespace Events {

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
