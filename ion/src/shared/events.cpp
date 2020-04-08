#include <ion/events.h>
#include <layout_events.h>

extern "C" {
#include <assert.h>
}

namespace Ion {
namespace Events {

const char * EventData::text() const {
  if (m_data == nullptr || m_data == k_textless) {
    return nullptr;
  }
  return m_data;
}

Event::Event(Keyboard::Key key, bool shift, bool alpha, bool lock) {
  // We're mapping a key, shift and alpha to an event
  // This can be a bit more complicated than it seems since we want to fall back:
  // for example, alpha-up is just plain up.
  // Fallback order :
  // shift-X -> X
  // alpha-X -> X
  // shift-alpha-X -> alpha-X -> X
  assert((int)key >= 0 && (int)key < Keyboard::NumberOfKeys);
  m_id = Events::None.m_id;

  int noFallbackOffsets[] = {0};
  int shiftFallbackOffsets[] = {PageSize, 0};
  int alphaFallbackOffsets[] = {2*PageSize, 0};
  int shiftAlphaFallbackOffsets[] = {3*PageSize, 2*PageSize, 0};

  int * fallbackOffsets[] = {noFallbackOffsets, shiftFallbackOffsets, alphaFallbackOffsets, shiftAlphaFallbackOffsets};

  int * fallbackOffset = fallbackOffsets[shift+2*alpha];
  int i=0;
  int offset = 0;
  do {
    offset = fallbackOffset[i++];
    m_id = offset + (int)key;
  } while (offset > 0 && !s_dataForEvent[m_id].isDefined() && m_id < 4*PageSize);

  //If we press percent in alphalock, change to backspace
  if (m_id == static_cast<uint8_t>(Ion::Events::Percent) && lock){
    m_id = static_cast<uint8_t>(Ion::Events::Backspace);
  }
  assert(m_id != Events::None.m_id);
}

const char * Event::text() const {
  if (m_id >= 4*PageSize) {
    return nullptr;
  }
  return s_dataForEvent[m_id].text();
}

bool Event::hasText() const {
  return text() != nullptr;
}

bool Event::isDefined() const {
  if (isKeyboardEvent()) {
    return s_dataForEvent[m_id].isDefined();
  } else {
    return (*this == None || *this == Termination || *this == USBEnumeration || *this == USBPlug || *this == BatteryCharging);
  }
}

}
}
