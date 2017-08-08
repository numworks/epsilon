#include <ion/events.h>

namespace Ion {
namespace Events {

ShiftAlphaStatus shiftAlphaStatus() {
  return ShiftAlphaStatus::Default;
}

bool isShiftActive() {
  return false;
}

bool isAlphaActive() {
  return false;
}

}
}
