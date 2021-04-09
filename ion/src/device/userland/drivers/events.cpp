#include <ion/events.h>
#include <layout_events.h>
#include <userland/drivers/svcall.h>

namespace Ion {
namespace Events {

size_t SVC_ATTRIBUTES copyText(uint8_t eventId, char * buffer, size_t bufferSize) {
  SVC(SVC_EVENTS_COPY_TEXT);
}

bool SVC_ATTRIBUTES isDefined(uint8_t eventId) {
  SVC(SVC_EVENTS_IS_DEFINED);
}

void SVC_ATTRIBUTES setShiftAlphaStatus(ShiftAlphaStatus s) {
  SVC(SVC_EVENTS_SET_SHIFT_ALPHA_STATUS);
}

ShiftAlphaStatus SVC_ATTRIBUTES shiftAlphaStatus() {
  SVC(SVC_EVENTS_SHIFT_ALPHA_STATUS);
}

int SVC_ATTRIBUTES repetitionFactor() {
  SVC(SVC_EVENTS_REPETITION_FACTOR);
}

Event SVC_ATTRIBUTES getEvent(int * timeout) {
  SVC(SVC_EVENTS_GET_EVENT);
}

}
}
