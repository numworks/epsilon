#include "main.h"
#include "platform.h"
#include "framebuffer.h"
#include "events.h"

#include <ion/events.h>
#include <layout_events.h>

#include <string.h>
#include <stdio.h>

void IonSimulatorEventsPushEvent(int eventNumber) {
}

static int sLogAfterNumberOfEvents = -1;
static int sEventCount = 0;

namespace Ion {
namespace Events {

Event getPlatformEvent() {
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
#if EPSILON_SIMULATOR_HAS_LIBPNG
  if (sEventCount++ > sLogAfterNumberOfEvents && sLogAfterNumberOfEvents >= 0) {
    char filename[32];
    sprintf(filename, "event%d.png", sEventCount);
    Ion::Simulator::Framebuffer::writeToFile(filename);
#ifndef NDEBUG
    printf("Event %d is %s\n", sEventCount, event.name());
#endif
  }
#endif
  return event;
}

}
}

namespace Ion {
namespace Simulator {
namespace Events {

void dumpEventCount(int i) {
  printf("Current event index: %d\n", sEventCount);
}

void logAfter(int numberOfEvents) {
  sLogAfterNumberOfEvents = numberOfEvents;
}

}
}
}
