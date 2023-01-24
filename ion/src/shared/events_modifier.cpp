#include "events_modifier.h"
#include <assert.h>

namespace Ion {
namespace Events {

OMG::GlobalBox<ModifierState> SharedModifierState;

// Implementation of public Ion::Events functions

ShiftAlphaStatus shiftAlphaStatus() {
  return SharedModifierState->shiftAlphaStatus();
}
void setShiftAlphaStatus(ShiftAlphaStatus s) {
  SharedModifierState->setShiftAlphaStatus(s);
}

int repetitionFactor() {
  return SharedModifierState->repetitionFactor();
}

int longPressCounter() {
  return SharedModifierState->longPressCounter();
}

// Internal functions

void ModifierState::removeShift() {
  if (m_shiftAlphaStatus == ShiftAlphaStatus::Shift) {
    m_shiftAlphaStatus = ShiftAlphaStatus::Default;
  } else if (m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha ) {
    m_shiftAlphaStatus = ShiftAlphaStatus::Alpha;
  } else if (m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock) {
    m_shiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
  }
}

void ModifierState::removeAlpha() {
  if (m_shiftAlphaStatus == ShiftAlphaStatus::Alpha || m_shiftAlphaStatus == ShiftAlphaStatus::AlphaLock) {
    m_shiftAlphaStatus = ShiftAlphaStatus::Default;
  } else if (m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock) {
    m_shiftAlphaStatus = ShiftAlphaStatus::Shift;
  }
}

bool ModifierState::isShiftActive() {
  return m_shiftAlphaStatus == ShiftAlphaStatus::Shift || m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
}

bool ModifierState::isAlphaActive() {
  return m_shiftAlphaStatus == ShiftAlphaStatus::Alpha || m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlpha || m_shiftAlphaStatus == ShiftAlphaStatus::AlphaLock || m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
}

bool ModifierState::isLockActive() {
  return m_shiftAlphaStatus == ShiftAlphaStatus::AlphaLock || m_shiftAlphaStatus == ShiftAlphaStatus::ShiftAlphaLock;
}

void ModifierState::setShiftAlphaStatus(ShiftAlphaStatus s) {
  if (static_cast<uint8_t>(s) >= static_cast<uint8_t>(ShiftAlphaStatus::NumberOfStatus)) {
    return;
  }
  m_shiftAlphaStatus = s;
}

bool ModifierState::wasShiftReleased(Keyboard::State state) {
  if (m_shiftIsHeldAndUsed && !state.keyDown(Keyboard::Key::Shift)) {
    m_shiftIsHeldAndUsed = false;
    removeShift();
    return true;
  }
  return false;
}

bool ModifierState::wasAlphaReleased(Keyboard::State state) {
  if (m_alphaIsHeldAndUsed && !state.keyDown(Keyboard::Key::Alpha)) {
    m_alphaIsHeldAndUsed = false;
    removeAlpha();
    return true;
  }
  return false;
}

void ModifierState::updateModifiersFromEvent(Event e, Keyboard::State state) {
  assert(e.isKeyboardEvent());
  bool shiftKeyDown = state.keyDown(Keyboard::Key::Shift);
  bool alphaKeyDown = state.keyDown(Keyboard::Key::Alpha);
  if (e != Shift && shiftKeyDown) {
    m_shiftIsHeldAndUsed = true;
  }
  if (e != Alpha && alphaKeyDown) {
    m_alphaIsHeldAndUsed = true;
  }
  switch (m_shiftAlphaStatus) {
    case ShiftAlphaStatus::Default:
      if (e == Shift) {
        m_shiftAlphaStatus = ShiftAlphaStatus::Shift;
      } else if (e == Alpha) {
        m_shiftAlphaStatus = ShiftAlphaStatus::Alpha;
      }
      break;
    case ShiftAlphaStatus::Shift:
      if (e == Shift) {
        m_shiftAlphaStatus = ShiftAlphaStatus::Default;
      } else if (e == Alpha) {
        m_shiftAlphaStatus = ShiftAlphaStatus::ShiftAlpha;
      } else {
        if (!shiftKeyDown) {
          m_shiftAlphaStatus = ShiftAlphaStatus::Default;
        }
      }
      break;
    case ShiftAlphaStatus::Alpha:
      if (e == Shift) {
        m_shiftAlphaStatus = ShiftAlphaStatus::ShiftAlpha;
      } else if (e == Alpha) {
        m_shiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
      } else {
        if (!alphaKeyDown) {
          m_shiftAlphaStatus = ShiftAlphaStatus::Default;
        }
      }
      break;
    case ShiftAlphaStatus::ShiftAlpha:
      if (e == Shift) {
        m_shiftAlphaStatus = ShiftAlphaStatus::Alpha;
      } else if (e == Alpha) {
        m_shiftAlphaStatus = ShiftAlphaStatus::ShiftAlphaLock;
      } else {
        if (!shiftKeyDown && !alphaKeyDown) {
          m_shiftAlphaStatus = ShiftAlphaStatus::Default;
        } else if (!shiftKeyDown) {
          m_shiftAlphaStatus = ShiftAlphaStatus::Alpha;
        } else if (!alphaKeyDown) {
          m_shiftAlphaStatus = ShiftAlphaStatus::Shift;
        } else {
          // Do nothing, both shift and alpha keys are down
        }
      }
      break;
    case ShiftAlphaStatus::AlphaLock:
      if (e == Shift) {
        m_shiftAlphaStatus = ShiftAlphaStatus::ShiftAlphaLock;
      } else if (e == Alpha) {
        m_shiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    case ShiftAlphaStatus::ShiftAlphaLock:
      if (e == Shift) {
        m_shiftAlphaStatus = ShiftAlphaStatus::AlphaLock;
      } else if (e == Alpha) {
        m_shiftAlphaStatus = ShiftAlphaStatus::Default;
      }
      break;
    default:
      assert(false);
  }
}

}
}
