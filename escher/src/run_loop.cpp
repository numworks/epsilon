#include <escher/run_loop.h>
#include <assert.h>

RunLoop::RunLoop() :
  m_time(0) {
}

int RunLoop::numberOfTimers() {
  return 0;
}

Timer * RunLoop::timerAtIndex(int i) {
  assert(false);
  return nullptr;
}

void RunLoop::run() {
  runWhile(nullptr, nullptr);
}

void RunLoop::runWhile(bool (*callback)(void * ctx), void * ctx) {
  while ((callback == nullptr || callback(ctx)) && step()) {
  }
}

bool RunLoop::step() {
  // Fetch the event, if any
  int eventDuration = Timer::TickDuration;
  int timeout = eventDuration;

  Ion::Events::Event event = Ion::Events::getEvent(&timeout);
  assert(event.isDefined());

  eventDuration -= timeout;
  assert(eventDuration >= 0);
  assert(eventDuration <= Timer::TickDuration);

  /* At this point, eventDuration contains the time it took to retrieve the
   * event. It can be zero, and is at most equal to the timeout value, Timer::
   * TickDuration.  The event returned can be None if nothing worth taking care
   * of happened. In other words, getEvent is a blocking call with a timeout. */

  m_time += eventDuration;

  if (m_time >= Timer::TickDuration) {
    m_time -= Timer::TickDuration;
    for (int i=0; i<numberOfTimers(); i++) {
      Timer * timer = timerAtIndex(i);
      if (timer->tick()) {
        dispatchEvent(Ion::Events::TimerFire);
      }
    }
  }

#if ESCHER_LOG_EVENTS_BINARY
  Ion::Console::writeChar(static_cast<uint8_t>(event));
#endif
#if ESCHER_LOG_EVENTS_NAME
  const char * name = event.name();
  if (name == nullptr) {
    name = "UNDEFINED";
  }
  Ion::Console::writeLine(name);
#endif

  if (event != Ion::Events::None) {
    dispatchEvent(event);
  }

  return event != Ion::Events::Termination;
}
