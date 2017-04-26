#include <ion/events.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"

namespace Ion {
namespace Events {

static int sLogAfterNumberOfEvents = -1;
static int sEventCount = 0;

Ion::Events::Event Ion::Events::getEvent(int * timeout) {
  int c = getchar();
  if (c == EOF) {
    printf("Finished processing %d events\n", sEventCount);
    exit(0);
  }
  Ion::Events::Event event(c);
  if (sEventCount++ > sLogAfterNumberOfEvents && sLogAfterNumberOfEvents >= 0) {
    char filename[32];
    sprintf(filename, "event%d.png", sEventCount);
    Ion::Display::Blackbox::writeFrameBufferToFile(filename);
    printf("Event %d is %s\n", sEventCount, event.name());
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
