#ifndef ION_SIMULATOR_JOURNAL_QUEUE_JOURNAL_H
#define ION_SIMULATOR_JOURNAL_QUEUE_JOURNAL_H

#include <ion/events.h>

#include <queue>

namespace Ion {
namespace Simulator {
namespace Journal {

class QueueJournal : public Ion::Events::Journal {
 public:
  void pushEvent(Ion::Events::Event e) override {
    if (e != Ion::Events::None) {
      m_eventStorage.push(e);
    }
  }
  Ion::Events::Event popEvent() override {
    if (isEmpty()) {
      return Ion::Events::None;
    }
    Ion::Events::Event e = m_eventStorage.front();
    m_eventStorage.pop();
    return e;
  }
  bool isEmpty() override { return m_eventStorage.empty(); }

 private:
  std::queue<Ion::Events::Event> m_eventStorage;
};

}  // namespace Journal
}  // namespace Simulator
}  // namespace Ion

#endif
