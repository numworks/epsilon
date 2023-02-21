#ifndef ION_SHARED_EVENTS_MODIFIER_H
#define ION_SHARED_EVENTS_MODIFIER_H

#include <ion/events.h>
#include <omg/global_box.h>

namespace Ion {
namespace Events {

class ModifierState {
 public:
  ModifierState()
      : m_shiftAlphaStatus(ShiftAlphaStatus::Default),
        m_shiftIsHeldAndUsed(false),
        m_alphaIsHeldAndUsed(false),
        m_longPressCounter(0),
        m_repetitionCounter(0) {}
  void removeShift();
  void removeAlpha();
  bool isShiftActive();
  bool isAlphaActive();
  bool isLockActive();
  ShiftAlphaStatus shiftAlphaStatus() const { return m_shiftAlphaStatus; }
  void setShiftAlphaStatus(ShiftAlphaStatus s);
  bool wasShiftReleased(Keyboard::State state);
  bool wasAlphaReleased(Keyboard::State state);
  void updateModifiersFromEvent(Event e, Keyboard::State state);
  int longPressCounter() { return m_longPressCounter; }
  int repetitionFactor() { return m_repetitionCounter; }
  void resetLongPress() { m_longPressCounter = 0; }
  void incrementLongPress() { m_longPressCounter++; }
  void incrementOrResetRepetition(bool increment);

 private:
  ShiftAlphaStatus m_shiftAlphaStatus;
  bool m_shiftIsHeldAndUsed;
  bool m_alphaIsHeldAndUsed;
  int m_longPressCounter;   // How long the event has been pressed (Computed
                            // value)
  int m_repetitionCounter;  // How much the event has been repeatedly pressed
                            // (Raw value)
};

extern OMG::GlobalBox<ModifierState> SharedModifierState;

}  // namespace Events
}  // namespace Ion

#endif
