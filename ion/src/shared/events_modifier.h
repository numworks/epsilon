#ifndef ION_SHARED_EVENTS_MODIFIER_H
#define ION_SHARED_EVENTS_MODIFIER_H

#include <ion/events.h>
#include <omg/global_box.h>

namespace Ion {
namespace Events {

class ModifierState {
 public:
  ModifierState()
      : m_shiftAlphaStatus(),
        m_shiftIsHeldAndUsed(false),
        m_alphaIsHeldAndUsed(false),
        m_longPressCounter(0),
        m_repetitionCounter(0) {}
  ShiftAlphaStatus* shiftAlphaStatus() { return &m_shiftAlphaStatus; }
  bool wasShiftReleased(Keyboard::State state);
  bool wasAlphaReleased(Keyboard::State state);
  void updateModifiersFromEvent(Event e, Keyboard::State state);
  int longPressCounter() const { return m_longPressCounter; }
  int repetitionFactor() const { return m_repetitionCounter; }
  void resetLongPress() { m_longPressCounter = 0; }
  void incrementLongPress() { m_longPressCounter++; }
  void incrementOrResetRepetition(bool increment);

 private:
  ShiftAlphaStatus m_shiftAlphaStatus;
  bool m_shiftIsHeldAndUsed;
  bool m_alphaIsHeldAndUsed;
  // How long the event has been pressed (Computed value)
  int m_longPressCounter;
  // How much the event has been repeatedly pressed (Raw value)
  int m_repetitionCounter;
};

extern OMG::GlobalBox<ModifierState> SharedModifierState;

}  // namespace Events
}  // namespace Ion

#endif
