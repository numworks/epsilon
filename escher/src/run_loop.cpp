#include <escher/run_loop.h>
#include <kandinsky/font.h>
#include <assert.h>

RunLoop::RunLoop() :
  m_time(0),
  m_firstTimer(nullptr)
{
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
    Timer * timer = m_firstTimer;
    while (timer) {
      if (timer->tick()) {
        dispatchEvent(Ion::Events::TimerFire);
      }
      timer = timer->next();
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
#if !PLATFORM_DEVICEdidStopAnimation
    if (event == Ion::Events::ExternalText && !KDFont::CanBeWrittenWithGlyphs(event.text())) {
      return true;
    }
#endif
    dispatchEvent(event);
  }

  return event != Ion::Events::Termination;
}

void RunLoop::addTimer(Timer * timer) {
  if (m_firstTimer == nullptr) {
    m_firstTimer = timer;
  } else {
    Timer * actual = m_firstTimer;
    while (actual->next()) {
      actual = actual->next();
    }
    actual->setNext(timer);
  }
}

void RunLoop::removeTimer(Timer * timer) {
  if (m_firstTimer == timer) {
    m_firstTimer = timer->next();
  } else {
    Timer * actual = m_firstTimer;
    while (actual->next() != timer) {
      actual = actual->next();
    }
    actual->setNext(timer->next());
  }
}
