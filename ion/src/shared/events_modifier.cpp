#include <ion.h>
#include <assert.h>

namespace Ion {
namespace Keyboard {

static ShiftAlphaStatus sShiftAlphaStatus = ShiftAlphaStatus::Default;

ShiftAlphaStatus shiftAlphaStatus() {
  return sShiftAlphaStatus;
}

bool isShiftActive() {
  return sShiftAlphaStatus == ShiftAlphaStatus::Shift || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || sShiftAlphaStatus == ShiftAlphaStatus::AlphaLockShift || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
}

bool isAlphaActive() {
  return sShiftAlphaStatus == ShiftAlphaStatus::Alpha || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || sShiftAlphaStatus == ShiftAlphaStatus::AlphaLock || sShiftAlphaStatus == ShiftAlphaStatus::AlphaLockShift || sShiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
;
}

void setShiftAlphaStatus(ShiftAlphaStatus s) {
  sShiftAlphaStatus = s;
}

void updateModifiersFromEvent(Events::Event e) {
  switch (sShiftAlphaStatus) {
    case ShiftAlphaStatus::Default:
      if (e == Events::Shift) {
        sShiftAlphaStatus = ShiftAlphaStatus::Shift;
      } else if (e == Events::Alpha) {
        sShiftAlphaStatus = ShiftAlphaStatus::Alpha;
      }
      break;
    case ShiftAlphaStatus::Shift:
      if (e == Events::Shift) {
        sShiftAlphaStatus = ShiftAlphaStatus::Default;
      } else if (e == Events::Alpha) {
        sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlpha;
      } else {
        sShiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    case ShiftAlphaStatus::Alpha:
      if (e == Events::Shift) {
	sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlpha;
      } else if (e == Events::Alpha) {
	sShiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
      } else {
	sShiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    case ShiftAlphaStatus::ShiftAlpha:
      if (e == Events::Shift) {
	sShiftAlphaStatus = ShiftAlphaStatus::Alpha;
      } else if (e == Events::Alpha) {
	sShiftAlphaStatus = ShiftAlphaStatus::ShiftAlphaLock;
      } else {
	sShiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    case ShiftAlphaStatus::AlphaLock:
      if (e == Events::Shift) {
	sShiftAlphaStatus = ShiftAlphaStatus::AlphaLockShift;
      } else if (e == Events::Alpha) {
	sShiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    case ShiftAlphaStatus::AlphaLockShift:
      if (e == Events::Alpha) {
	sShiftAlphaStatus = ShiftAlphaStatus::Shift;
      } else {
	sShiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
      }
      break;
    case ShiftAlphaStatus::ShiftAlphaLock:
      if (e == Events::Alpha) {
	sShiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
  }
}

}
}
