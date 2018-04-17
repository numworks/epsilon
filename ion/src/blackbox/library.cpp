// Turn Epsilon into a library
#include "library.h"
#include "display.h"
#include <ion.h>
#include <mutex>
#include <condition_variable>

void PREFIXED(main)() {
  Ion::Display::Blackbox::setFrameBufferActive(true);
  ion_main(0, nullptr);
}

static std::mutex m;
static std::condition_variable cv;
static Ion::Events::Event sEvent = Ion::Events::None;

enum class State {
  WaitingForEvent,
  EventAvailable,
  Processing,
  Processed
};

static State state = State::WaitingForEvent;

Ion::Events::Event Ion::Events::getEvent(int * timeout) {
  if (state == State::Processing) {
    std::lock_guard<std::mutex> lk(m);
    state = State::Processed;
    cv.notify_one();
  }

  std::unique_lock<std::mutex> lk(m);
  cv.wait(lk, []{return (state == State::EventAvailable);});
  state = State::Processing;

  lk.unlock();
  cv.notify_one();

  return sEvent;
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

  sEvent = e;
  {
    std::lock_guard<std::mutex> lk(m);
    state = State::EventAvailable;
  }
  cv.notify_one();
}

void PREFIXED(wait_event_processed)() {
  if (state == State::EventAvailable || state == State::Processing) {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, []{return state == State::Processed;});
    state = State::WaitingForEvent;
  }
  cv.notify_one();
}


const KDColor * PREFIXED(frame_buffer)() {
  return Ion::Display::Blackbox::frameBufferAddress();
}

void PREFIXED(write_frame_buffer_to_file)(const char * c) {
  Ion::Display::Blackbox::writeFrameBufferToFile(c);
}
