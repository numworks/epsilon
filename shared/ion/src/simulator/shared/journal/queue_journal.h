#pragma once

#include <ion/clipboard.h>
#include <ion/events.h>

#include <queue>

#include "../events.h"

namespace Ion {
namespace Simulator {
namespace Journal {

class QueueJournal : public Ion::Events::Journal {
 public:
  void pushEvent(Ion::Events::Event e) override {
#if ION_KEYBOARD_RICH
    if (e == Ion::Events::Paste) {
      const char* systemBuffer = Ion::Clipboard::read();
      if (systemBuffer) {
        // Changed a Paste that uses the system clipboard into an ExternalText
        // to preserve pasted content
        size_t len = strlen(systemBuffer);
        // 1 <=len<= 9 = 1 event
        // 10<=len<=18 = 2 events
        // 19<=len<=27 = 3 events
        for (int i = 0;
             i < (len + Ion::Events::sharedExternalTextBufferSize - 2) /
                     (Ion::Events::sharedExternalTextBufferSize - 1);
             ++i) {
          m_eventStorage.push(Ion::Events::ExternalText);
          AdditionalData data;
          strlcpy(data.text, systemBuffer,
                  Ion::Events::sharedExternalTextBufferSize);
          m_externalTextStorage.push(data);
          systemBuffer += Ion::Events::sharedExternalTextBufferSize - 1;
        }
        return;
      }
    }
#endif
    if (e != Ion::Events::None) {
      m_eventStorage.push(e);
    }
    if (e == Ion::Events::ExternalText) {
      AdditionalData data;
      strlcpy(data.text, Ion::Events::sharedExternalTextBuffer(),
              Ion::Events::sharedExternalTextBufferSize);
      m_externalTextStorage.push(data);
    }
  }
  Ion::Events::Event popEvent() override {
    if (isEmpty()) {
      return Ion::Events::None;
    }
    Ion::Events::Event e = m_eventStorage.front();
    m_eventStorage.pop();
    if (e == Ion::Events::ExternalText) {
      assert(!m_externalTextStorage.empty());
      strlcpy(Ion::Events::sharedExternalTextBuffer(),
              m_externalTextStorage.front().text,
              Ion::Events::sharedExternalTextBufferSize);
      m_externalTextStorage.pop();
    }
    return e;
  }
  bool isEmpty() override {
    assert(m_externalTextStorage.empty() || !m_eventStorage.empty());
    return m_eventStorage.empty();
  }

 private:
  std::queue<Ion::Events::Event> m_eventStorage;
  struct AdditionalData {
    char text[Ion::Events::sharedExternalTextBufferSize];
  };
  std::queue<AdditionalData> m_externalTextStorage;
};

}  // namespace Journal
}  // namespace Simulator
}  // namespace Ion
