#include <userland/drivers/svcall.h>
#include <ion/events.h>

namespace Ion {
namespace Events {

void SVC_ATTRIBUTES getPlatformEventSVC(Event * e) {
  SVC(SVC_EVENTS_GET_PLATFORM_EVENT);
}

Event getPlatformEvent() {
  Event e;
  getPlatformEventSVC(&e);
  return e;
}

}
}
