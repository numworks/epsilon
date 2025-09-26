#include "../shared/journal.h"

#include <emscripten.h>

#include "../shared/journal/queue_journal.h"

namespace Ion {
namespace Simulator {
namespace Journal {

using Ion::Events::Event;
using Ion::Events::None;

static inline void callOnEpsilonIdle() {
  // clang-format off
  EM_ASM({
    if (typeof Module.onEpsilonIdle === "function") {
      Module.onEpsilonIdle();
    }
  });
  // clang-format on
}

static inline void sendEventToJS(uint8_t event) {
  // clang-format off
  EM_ASM({
    if (typeof Module.onIonEvent === "function") {
      Module.onIonEvent($0, true);
    }
  }, event);
  // clang-format on
}

static inline void sendRawDataToJS(uint8_t data) {
  // clang-format off
  EM_ASM({
    if (typeof Module.onIonEvent === "function") {
      Module.onIonEvent($0, false);
    }
  }, data);
  // clang-format on
}

static inline void sendRawDataToJS(const char* data) {
  while (true) {
    sendRawDataToJS(*data);
    if (*data == 0) {
      break;
    }
    ++data;
  }
}

class LogJournal : public Ion::Events::Journal {
 public:
  void pushEvent(Event e) override {
    static bool lastEventWasNone = false;
#if ION_KEYBOARD_RICH
    if (e == Ion::Events::Paste) {
      const char* systemBuffer = Ion::Clipboard::read();
      if (systemBuffer) {
        sendEventToJS(Events::Event(Events::ExternalText));
        sendRawDataToJS(systemBuffer);
      } else {
        sendEventToJS(e);
      }
    } else
#endif
        if (e == Ion::Events::ExternalText) {
      sendEventToJS(e);
      sendRawDataToJS(Ion::Events::sharedExternalTextBuffer());
    } else if (e != Ion::Events::None) {
      sendEventToJS(e);
    }
    if (e != None) {
      lastEventWasNone = false;
    } else {
      if (!lastEventWasNone) {
        callOnEpsilonIdle();
        lastEventWasNone = true;
      }
    }
  }
  Event popEvent() override { return None; }
  bool isEmpty() override { return true; }
};

void init() { Events::logTo(logJournal()); }

Events::Journal* replayJournal() {
  static QueueJournal journal;
  return &journal;
}

Ion::Events::Journal* logJournal() {
  static LogJournal journal;
  return &journal;
}

}  // namespace Journal
}  // namespace Simulator
}  // namespace Ion
