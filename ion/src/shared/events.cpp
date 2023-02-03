#include "events.h"
#include "events_modifier.h"
#include <ion/keyboard.h>
#include <ion/timing.h>
#include <ion/keyboard/layout_events.h>
#include <limits.h>
#include <algorithm>
#include <string.h>

extern "C" {
#include <assert.h>
}

namespace Ion {
namespace Events {

OMG::GlobalBox<State> SharedState;

// Implementation of public Ion::Events functions

const char * EventData::text() const {
  if (m_data == nullptr || m_data[0] == 0) {
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
  assert(static_cast<int>(key) >= 0 && static_cast<int>(key) < Keyboard::NumberOfKeys);
  m_id = Events::None.m_id;

  int noFallbackOffsets[] = {k_plainEventsOffset};
  int shiftFallbackOffsets[] = {k_shiftEventsOffset, k_plainEventsOffset};
  int alphaFallbackOffsets[] = {k_alphaEventsOffset, k_plainEventsOffset};
  int shiftAlphaFallbackOffsets[] = {k_shiftAlphaEventsOffset, k_alphaEventsOffset, k_plainEventsOffset};

  int * fallbackOffsets[] = {noFallbackOffsets, shiftFallbackOffsets, alphaFallbackOffsets, shiftAlphaFallbackOffsets};

  int * fallbackOffset = fallbackOffsets[shift + 2 * alpha];
  int i = 0;
  int offset = k_plainEventsOffset;
  do {
    offset = fallbackOffset[i++];
    m_id = offset + (int)key;
  } while (offset > k_plainEventsOffset && !s_dataForEvent[m_id].isDefined() && m_id < k_specialEventsOffset);

  // If we press percent in alphalock, change to backspace
  if (m_id == static_cast<uint8_t>(Ion::Events::Percent) && lock){
    m_id = static_cast<uint8_t>(Ion::Events::Backspace);
  }
  assert(m_id != Events::None.m_id);
}

const char * Event::defaultText() const {
  /* As the ExternalText event is only available on the simulator, we save a
   * comparison by not handling it on the device. */
  if (m_id >= k_specialEventsOffset) {
    return nullptr;
  }
  return s_dataForEvent[m_id].text();
}

// Internal functions

Event State::privateSharedGetEvent(int * timeout) {
  constexpr int delayBeforeRepeat = 200;
  constexpr int delayBetweenRepeat = 50;
  assert(*timeout > delayBeforeRepeat);
  assert(*timeout > delayBetweenRepeat);

  int startTime = Ion::Timing::millis();
  while (true) {
    if (handlePreemption(false)) {
      return None;
    }

    Event platformEvent = getPlatformEvent();
    if (platformEvent != None) {
      /* WARNING: events that can repeat should not be handled here since
       * m_lastKeyboardState is not updated and is used to know if the event
       * is repeated at line "bool isRepeatableEvent = ..."
       * On device this is not a problem.
       * On simulator, most repeating events are handled as Keyboard::States
       * because they are listed in Keyboard::sKeyPairs, except for Backspace.
       * This can lead to Backspace repeating infinitely.
       * Namely, if you start a simulator and press Backspace as your first
       * input, it will repeat infinitely.
       * TODO: Find a solution. Either add Backspace to sKeyPairs and find
       * another solution for the shift+backspace issue (see comment in
       * keyboard.cpp). Or find a way to register computer keyboard state
       * in m_lastKeyboardState, by using SDL_Event::type::KEY_UP */
      SharedModifierState->incrementOrResetRepetition(platformEvent == m_lastEvent);
      m_lastEvent = platformEvent;
      return platformEvent;
    }

    bool lock = SharedModifierState->isLockActive();
    uint64_t keysSeenTransitioningFromUpToDown;
    Keyboard::State state;
    while ((state = Keyboard::popState()) != Keyboard::State(-1)) {
      m_currentKeyboardState = state;
      keysSeenTransitioningFromUpToDown = state & m_keysSeenUp;
      m_keysSeenUp = ~state;

      if (SharedModifierState->wasShiftReleased(state)) {
        return Event::PlainKey(Keyboard::Key::Shift);
      }
      if (SharedModifierState->wasAlphaReleased(state)) {
        return Event::PlainKey(Keyboard::Key::Alpha);
      }

      if (keysSeenTransitioningFromUpToDown != 0) {
        SharedModifierState->resetLongPress();
        /* The key that triggered the event corresponds to the first non-zero bit
         * in "match". This is a rather simple logic operation for the which many
         * processors have an instruction (ARM thumb uses CLZ).
         * Unfortunately there's no way to express this in standard C, so we have
         * to resort to using a builtin function. */
        Keyboard::Key key = (Keyboard::Key)(63-__builtin_clzll(keysSeenTransitioningFromUpToDown));
        didPressNewKey();
        m_lastEventShift = SharedModifierState->isShiftActive();
        m_lastEventAlpha = SharedModifierState->isAlphaActive();
        Event event(key, m_lastEventShift, m_lastEventAlpha, lock);
        SharedModifierState->updateModifiersFromEvent(event, state);
        SharedModifierState->incrementOrResetRepetition(event == m_lastEvent);
        m_lastEvent = event;
        m_lastKeyboardState = state;
        return event;
      }
    }

    int maximumDelay = *timeout;
    int delayForRepeat = INT_MAX;
    bool isRepeatableEvent = m_lastEvent.isRepeatable()
                          && m_lastKeyboardState == m_currentKeyboardState
                          && m_lastEventShift == (m_currentKeyboardState.keyDown(Keyboard::Key::Shift) || (m_lastEventShift && lock))
                          && m_lastEventAlpha == (m_currentKeyboardState.keyDown(Keyboard::Key::Alpha) || lock);
    if (isRepeatableEvent) {
      delayForRepeat = isRepeating() ? delayBetweenRepeat : delayBeforeRepeat;
      maximumDelay = std::min(maximumDelay, delayForRepeat);
    }

    if (waitForInterruptingEvent(maximumDelay, timeout)) {
      continue;
    }

    if (*timeout == 0) {
      SharedModifierState->resetLongPress();
      return None;
    }

    int elapsedTime = Ion::Timing::millis() - startTime;

    /* At this point, we know that keysSeenTransitioningFromUpToDown has
     * always been zero. In other words, no new key has been pressed. */
    if (elapsedTime >= delayForRepeat) {
      assert(isRepeatableEvent);
      SharedModifierState->incrementLongPress();
      return m_lastEvent;
    }

  }
}

Event State::sharedGetEvent(int * timeout) {
  Event event = privateSharedGetEvent(timeout);
  if (event == None) {
    if (!m_idleWasSent) {
      m_idleWasSent = true;
      return Idle;
    }
  } else {
    m_idleWasSent = false;
  }
  return event;
}

void State::resetKeyboardState() {
  m_keysSeenUp = -1;
  /* Set the keyboard state of reference to -1 to prevent event repetition. */
  m_lastKeyboardState = -1;
}

Event sharedGetEvent(int * timeout) {
  return SharedState->sharedGetEvent(timeout);
}

size_t sharedCopyText(uint8_t eventId, char * buffer, size_t bufferSize) {
  Event e(eventId);
  if (e.text()) {
    return strlcpy(buffer, e.text(), bufferSize);
  } else {
    return 0;
  }
}

bool sharedIsDefined(uint8_t eventId) {
  Event e(eventId);
  if (e.isKeyboardEvent()) {
    return s_dataForEvent[static_cast<uint8_t>(e)].isDefined();
  } else {
    return (e == None || e == Termination || e == USBEnumeration || e == USBPlug || e == BatteryCharging || e == ExternalText || e == Idle);
  }
}

void resetKeyboardState() {
  SharedState->resetKeyboardState();
}

}
}
