#ifndef ION_SHARED_EVENTS_H
#define ION_SHARED_EVENTS_H

#include <ion/events.h>
#include <ion/keyboard.h>
#include <omg/global_box.h>

namespace Ion {
namespace Events {

class State {
public:
  State() :
    m_lastEvent(Events::None),
    m_lastKeyboardState(0),
    m_currentKeyboardState(0),
    m_keysSeenUp(-1),
    m_lastEventShift(false),
    m_lastEventAlpha(false),
    m_idleWasSent(false)
  {}
  Event sharedGetEvent(int * timeout);
  void resetKeyboardState();
private:
  Event privateSharedGetEvent(int * timeout);

  Event m_lastEvent;
  Keyboard::State m_lastKeyboardState;
  Keyboard::State m_currentKeyboardState;
  uint64_t m_keysSeenUp;
  bool m_lastEventShift;
  bool m_lastEventAlpha;
  bool m_idleWasSent;
};

extern OMG::GlobalBox<State> SharedState;

// Functions defined in events.cpp
Event sharedGetEvent(int * timeout);
size_t sharedCopyText(uint8_t eventId, char * buffer, size_t bufferSize);
bool sharedIsDefined(uint8_t eventId);
void resetKeyboardState();

// Externally-provided functions
bool handlePreemption(bool stalling);
void setPreemptiveKeyboardState(Keyboard::State s);
void resetPreemptiveKeyboardState();

Event getPlatformEvent();
void didPressNewKey();
void openURL(const char * url);
bool waitForInterruptingEvent(int maximumDelay, int * timeout);

}
}

#endif
