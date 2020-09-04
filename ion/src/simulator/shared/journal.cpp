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
    Event e = m_eventStorage.front();
    m_eventStorage.pop();
    return e;
  }
private:
  std::queue<Event> m_eventStorage;
};

void init() {
  Ion::Events::logTo(logJournal());
}

Ion::Events::Journal * replayJournal() {
  static Journal journal;
  return &journal;
}

Ion::Events::Journal * logJournal() {
  static Journal journal;
  return &journal;
}

}
}
}
