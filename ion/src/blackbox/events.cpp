#include <ion/events.h>
#include <stdio.h>
#include <stdlib.h>
#include <layout_events.h>
#include "display.h"

namespace Ion {
namespace Events {

static int sLogAfterNumberOfEvents = -1;
static int sEventCount = 0;

Event getEvent(int * timeout) {
  Ion::Events::Event event = Ion::Events::None;
  while (!(event.isDefined() && event.isKeyboardEvent())) {
    int c = getchar();
    if (c == EOF) {
      printf("Finished processing %d events\n", sEventCount);
      event = Ion::Events::Termination;
      break;
    }
    event = Ion::Events::Event(c);
  }
  if (sEventCount++ > sLogAfterNumberOfEvents && sLogAfterNumberOfEvents >= 0) {
    char filename[32];
    sprintf(filename, "event%d.png", sEventCount);
    Ion::Display::Blackbox::writeFrameBufferToFile(filename);
#ifndef NDEBUG
    printf("Event %d is %s\n", sEventCount, event.name());
#endif
  }
  return event;
}

namespace Blackbox {

void dumpEventCount(int i) {
  printf("Current event index: %d\n", sEventCount);
}

void logAfter(int numberOfEvents) {
  sLogAfterNumberOfEvents = numberOfEvents;
}

}

}
}
