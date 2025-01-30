#ifndef ION_SIMULATOR_JOURNAL_QUEUE_JOURNAL_H
#define ION_SIMULATOR_JOURNAL_QUEUE_JOURNAL_H

#include <ion/events.h>

#include <queue>

#include "../events.h"

namespace Ion {
namespace Simulator {
namespace Journal {

class QueueJournal : public Ion::Events::Journal {
 public:
  void pushEvent(Ion::Events::Event e) override {
    if (e != Ion::Events::None) {
      m_eventStorage.push(e);
    }
    if (e == Ion::Events::ExternalText || e == Ion::Events::ExternalChar) {
      AdditionalData data;
      if (e == Ion::Events::ExternalChar) {
        data.text[0] = Ion::Events::sharedExternalTextBuffer()[0];
      } else {
        strlcpy(data.text, Ion::Events::sharedExternalTextBuffer(),
                Ion::Events::sharedExternalTextBufferSize);
      }
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
      strlcpy(Ion::Events::sharedExternalTextBuffer(),
              m_externalTextStorage.front().text,
              Ion::Events::sharedExternalTextBufferSize);
      m_externalTextStorage.pop();
    } else if (e == Ion::Events::ExternalChar) {
      Ion::Events::sharedExternalTextBuffer()[0] =
          m_externalTextStorage.front().text[0];
      Ion::Events::sharedExternalTextBuffer()[1] = 0;
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

#endif
