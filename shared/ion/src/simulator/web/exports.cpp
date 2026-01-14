#include "exports.h"

#include <ion.h>

#include "../shared/display.h"
#include "../shared/events.h"
#include "../shared/journal.h"
#include "../shared/keyboard.h"
#include "../shared/state_file.h"
#include "../shared/window.h"

const char* IonSoftwareVersion() { return Ion::softwareVersion(); }

const char* IonPatchLevel() { return Ion::commitHash(); }

void IonSimulatorKeyboardKeyDown(int keyNumber) {
  Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(keyNumber);
  Ion::Simulator::Keyboard::keyDown(key);
}

void IonSimulatorKeyboardKeyUp(int keyNumber) {
  Ion::Keyboard::Key key = static_cast<Ion::Keyboard::Key>(keyNumber);
  Ion::Simulator::Keyboard::keyUp(key);
}

void IonSimulatorEventsPushEvent(int eventNumber) {
  Ion::Events::Journal* j = Ion::Simulator::Journal::replayJournal();
  if (j != nullptr) {
    Ion::Events::Event event(eventNumber);
    j->pushEvent(event);
    Ion::Events::replayFrom(j);
  }
}

/* Fill the replayJournal with events from the [eventBufferAsChar] of size
 * [length].
 * Starting at event [fromIndex] included and stopping at [toIndex] exluded.
 * This index do not refer to indexes of [eventBufferAsChar] due to ExternalText
 * event possible offset. They instead refer to a global eventCounter.
 * With [fromIndex = 3, toIndex = 10]:
 * The first 3 events of the buffer will be skipped, the following 7 (10-3)
 * events will be replayed.
 * */
void IonSimulatorEventsPushEventArray(const char* eventBufferAsChar, int length,
                                      int fromIndex, int toIndex) {
  const uint8_t* eventBuffer =
      reinterpret_cast<const uint8_t*>(eventBufferAsChar);
  const uint8_t* bufferEnd = eventBuffer + length;

  if (toIndex < 0) {
    toIndex = length;
  }
  int currentEventIndex = 0;
  /* NOTE: A long ExternalText can be split into many smaller one by
   * [pushEventFromMemory]. Event when this happend, [currentEventIndex]
   * should only increase by 1 */
  while (eventBuffer < bufferEnd && currentEventIndex < toIndex) {
    uint8_t ch = *eventBuffer;
    ++eventBuffer;
    Ion::Simulator::StateFile::pushEventFromMemory(ch, &eventBuffer, bufferEnd);
    if (currentEventIndex < fromIndex) {
      while (!Ion::Simulator::Journal::replayJournal()->isEmpty()) {
        /* NOTE: A long ExternalText may create more than one event in the
         * journal. So cleaning until empty is necessary */
        Ion::Simulator::Journal::replayJournal()->popEvent();
      }
    }
    currentEventIndex += 1;
  }
  Ion::Events::replayFrom(Ion::Simulator::Journal::replayJournal());
}

/* Return the number of events in the [eventBuffer].
 * This method takes into account the possible ExternalText events */
int IonSimulatorNumberOfEventsInArray(const char* eventBuffer, int length) {
  int numberOfEvents = 0;
  const char* end = eventBuffer + length;
  bool skipUntilNullByte = false;
  do {
    if (skipUntilNullByte) {
      skipUntilNullByte = *eventBuffer != 0;
      continue;
    }
    ++numberOfEvents;
    if (Ion::Events::Event(*eventBuffer) == Ion::Events::ExternalText) {
      skipUntilNullByte = true;
    }
  } while (++eventBuffer != end);
  return numberOfEvents;
}
