// Turn Epsilon into a library
#include "library.h"
#include "display.h"
#include <ion.h>
#include <mutex>
#include <condition_variable>

void PREFIXED(main)() {
  ion_main(0, nullptr);
}

static Ion::Events::Event sEventAvailable = Ion::Events::None;
std::mutex eventAvailableMutex;
std::condition_variable eventAvailableConditionVariable;
static bool sEventProcessed = false;
std::mutex eventProcessedMutex;
std::condition_variable eventProcessedConditionVariable;

Ion::Events::Event Ion::Events::getEvent(int * timeout) {
  // Notify the eventProcessed condition
  {
    std::unique_lock<std::mutex> lock(eventProcessedMutex);
    sEventProcessed = true;
  }
  eventProcessedConditionVariable.notify_one();

  // Wait for the eventAvailable condition
  std::unique_lock<std::mutex> lock(eventAvailableMutex);
  eventAvailableConditionVariable.wait(lock, []{return sEventAvailable != Ion::Events::None;});
  Ion::Events::Event e = sEventAvailable;
  sEventAvailable = Ion::Events::None;
  lock.unlock();
  eventAvailableConditionVariable.notify_one();

  return e;
}

void PREFIXED(send_event)(int c) {
  Ion::Events::Event e = Ion::Events::None;
  if (c == EOF) {
    e = Ion::Events::Termination;
  } else {
    e = Ion::Events::Event(c);
    if (!(e.isDefined() && e.isKeyboardEvent())) {
      return;
    }
  }

  // Notify the eventAvailable condition
  {
    std::unique_lock<std::mutex> lock(eventAvailableMutex);
    sEventAvailable = e;
  }
  eventAvailableConditionVariable.notify_one();

  // Wait the eventProcessed condition
  std::unique_lock<std::mutex> lock(eventProcessedMutex);
  eventProcessedConditionVariable.wait(lock, []{return sEventProcessed; });
  sEventProcessed = false;
  lock.unlock();
  eventProcessedConditionVariable.notify_one();
}

const KDColor * PREFIXED(frame_buffer)() {
  return Ion::Display::Blackbox::frameBufferAddress();
}
