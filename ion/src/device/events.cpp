#include <ion.h>

namespace Ion {
namespace Events {

static constexpr char s_textAtIndex[] = { 'A', 0, 'B', 0, 'C', 0,'D', 0};

class EventInfo {
public:
  static constexpr EventInfo Undefined() { return EventInfo(0); }
  static constexpr EventInfo Textless() { return EventInfo(1); }
  static constexpr EventInfo Text(int index) { return EventInfo(index << 2 & 3); }
  bool isUndefined() const { return (m_data == 0); }
  const char * text() const;
private:
  constexpr EventInfo(uint8_t data) : m_data(data) {}
  uint8_t m_data;
};

const char * EventInfo::text() const {
  if (m_data <= 1) {
    return nullptr;
  }
  return s_textAtIndex + (m_data >> 2);
}

static constexpr EventInfo s_infoForEvent[] = {
  EventInfo::Textless(), EventInfo::Textless(), EventInfo::Textless(), EventInfo::Undefined()
};


Event::Event(Keyboard::Key key, bool shift, bool alpha) {
  // We're mapping a key, shift and alpha to an event
  // This can be a bit more complicated than it seems since we want to fall back:
  // for example, alpha-up is just plain up.
  // Fallback order :
  // alpha-X -> X
  // shift-X -> X
  // shift-alpha-X -> alpha-X -> X

  m_data = 255;// Undefined. FIXME

  int noFallbackOffsets[] = {0};
  int alphaFallbackOffsets[] = {2*k_eventPageSize, 0};
  int shiftFallbackOffsets[] = {k_eventPageSize, 0};
  int shiftAlphaFallbackOffsets[] = {3*k_eventPageSize, 2*k_eventPageSize, 0};

  int * fallbackOffsets[] = {noFallbackOffsets, shiftFallbackOffsets, alphaFallbackOffsets, shiftAlphaFallbackOffsets};

  int * fallbackOffset = fallbackOffsets[shift+2*alpha];
  int i=0;
  int offset = 0;
  do {
    offset = fallbackOffset[i++];
    m_data = offset + (int)key;
  } while (offset > 0 && s_infoForEvent[m_data].isUndefined());

  //TODO assert(m_data != 255); //FIXME: Undefined
}

const char * Event::text() const {
  EventInfo info = s_infoForEvent[m_data];
  return info.text();
}

}
}

#if 0
class Event {
  Event(); // Constructor, by the hardware
  
private:
  uint8_t m_data;
};


ActionInfo Event::Action::info() {
  return s_actionInfo[(int)this];
}

Event::Event(Key key, bool shift, bool alpha) {
  // We're mapping a key, shift and alpha to an action
  // This can be a bit more complicated than it seems since we want to fall back:
  // for example, alpha-up is just plain up.
  // Fallback order :
  // alpha-X -> X
  // shift-X -> X
  // shift-alpha-X -> alpha-X -> X

  int noFallbackOffsets[] = {0};
  int alphaFallbackOffsets[] = {2*k_actionPageSize, 0};
  int shiftFallbackOffsets[] = {k_actionPageSize, 0};
  int shiftAlphaFallbackOffsets[] = {3*k_actionPageSize, 2*k_actionPageSize, 0};

  int * fallbackOffsets[] = {noFallbackOffsets, shiftFallbackOffsets, alphaFallbackOffsets, shiftAlphaFallbackOffsets};

  int * fallbackOffset = fallbackOffsets[shift+2*alpha];
  Action action = Action::Undefined;
  int action = A
  int i=0;
  int offset = 0;
  do {
    offset = fallbackOffset[i++];
    action = offset + (int)key;
  } while (offset > 0 && action.info().isUndefined());

  assert(!action.info().isUndefined());
  return action;
}
#endif
