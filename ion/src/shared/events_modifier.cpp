#include "events_modifier.h"

#include <assert.h>

namespace Ion {
namespace Events {

OMG::GlobalBox<ModifierState> SharedModifierState;

// Implementation of public Ion::Events functions

ShiftAlphaStatus shiftAlphaStatus() {
  return *SharedModifierState->shiftAlphaStatus();
}
void setShiftAlphaStatus(ShiftAlphaStatus s) {
  *SharedModifierState->shiftAlphaStatus() = s;
}

int repetitionFactor() { return SharedModifierState->repetitionFactor(); }

int longPressCounter() { return SharedModifierState->longPressCounter(); }

// Internal functions
bool ModifierState::wasShiftReleased(Keyboard::State state) {
  if (m_shiftIsHeldAndUsed && !state.keyDown(Keyboard::Key::Shift)) {
    m_shiftIsHeldAndUsed = false;
    m_shiftAlphaStatus.removeShift();
    return true;
  }
  return false;
}

bool ModifierState::wasAlphaReleased(Keyboard::State state) {
  if (m_alphaIsHeldAndUsed && !state.keyDown(Keyboard::Key::Alpha)) {
    m_alphaIsHeldAndUsed = false;
    m_shiftAlphaStatus.removeAlpha();
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
  if (e == Shift) {
    m_shiftAlphaStatus.toggleShift();
    return;
  }
  if (e == Alpha) {
    m_shiftAlphaStatus.cycleAlpha();
    return;
  }
  if (m_shiftAlphaStatus.alphaIsLocked()) {
    return;
  }
  if (!shiftKeyDown) {
    m_shiftAlphaStatus.removeShift();
  }
  if (!alphaKeyDown) {
    m_shiftAlphaStatus.removeAlpha();
  }
}

void ModifierState::incrementOrResetRepetition(bool increment) {
  m_repetitionCounter = increment ? m_repetitionCounter + 1 : 0;
}

}  // namespace Events
}  // namespace Ion
