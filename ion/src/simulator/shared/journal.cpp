#include "journal.h"
#include <queue>

namespace Ion {
namespace Simulator {
namespace Journal {

using Ion::Events::Event;
class Journal : public Ion::Events::Journal {
public:
  void pushEvent(Event e) override {
    m_eventStorage.push(e);
  }
  virtual Event popEvent() override {
    if (isEmpty()) {
      return Ion::Events::None;
    }
    Event e = m_eventStorage.front();
    m_eventStorage.pop();
    return e;
  }
  virtual bool isEmpty() override {
    return m_eventStorage.empty();
  }
private:
  std::queue<Event> m_eventStorage;
};

void init() {
  Events::logTo(logJournal());
}

Events::Journal * replayJournal() {
  static Journal journal;
  return &journal;
}

Events::Journal * logJournal() {
  static Journal journal;
  return &journal;
}

}
}
}
