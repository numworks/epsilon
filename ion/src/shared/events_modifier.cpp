#include <ion.h>
#include <assert.h>

namespace Ion {
namespace Events {

static ShiftAlphaStatus sShiftAlphaStatus = ShiftAlphaStatus::Default;

ShiftAlphaStatus shiftAlphaStatus() {
  return sShiftAlphaStatus;
}

bool isShiftActive() {
  return sShiftAlphaStatus == ShiftAlphaStatus::Shift || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
}

bool isAlphaActive() {
  return sShiftAlphaStatus == ShiftAlphaStatus::Alpha || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || sShiftAlphaStatus == ShiftAlphaStatus::AlphaLock || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
;
}

void setShiftAlphaStatus(ShiftAlphaStatus s) {
  sShiftAlphaStatus = s;
}

void updateModifiersFromEvent(Event e) {
  assert(e.isKeyboardEvent());
  switch (sShiftAlphaStatus) {
    case ShiftAlphaStatus::Default:
      if (e == Shift) {
        sShiftAlphaStatus = ShiftAlphaStatus::Shift;
      } else if (e == Alpha) {
        sShiftAlphaStatus = ShiftAlphaStatus::Alpha;
      }
      break;
    case ShiftAlphaStatus::Shift:
      if (e == Shift) {
        sShiftAlphaStatus = ShiftAlphaStatus::Default;
      } else if (e == Alpha) {
        sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlpha;
      } else {
        sShiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    case ShiftAlphaStatus::Alpha:
      if (e == Shift) {
          sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlpha;
        } else if (e == Alpha) {
          sShiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
        } else {
          sShiftAlphaStatus = ShiftAlphaStatus::Default;
        }
        break;
      case ShiftAlphaStatus::ShiftAlpha:
        if (e == Shift) {
          sShiftAlphaStatus = ShiftAlphaStatus::Alpha;
        } else if (e == Alpha) {
          sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlphaLock;
        } else {
          sShiftAlphaStatus = ShiftAlphaStatus::Default;
        }
        break;
      case ShiftAlphaStatus::AlphaLock:
        if (e == Shift) {
          sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlphaLock;
        } else if (e == Alpha) {
          sShiftAlphaStatus = ShiftAlphaStatus::Default;
        }
        break;
      case ShiftAlphaStatus::ShiftAlphaLock:
        if (e == Shift) {
          sShiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
        } else if (e == Alpha) {
          sShiftAlphaStatus = ShiftAlphaStatus::Default;
        }
        break;
    }
}

}
}
