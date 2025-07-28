#include "journal.h"

#include <queue>

#include "journal/queue_journal.h"

namespace Ion {
namespace Simulator {
namespace Journal {

void init() { Events::logTo(logJournal()); }

Events::Journal* replayJournal() {
  static QueueJournal journal;
  return &journal;
}

Events::Journal* logJournal() {
  static QueueJournal journal;
  return &journal;
}

}  // namespace Journal
}  // namespace Simulator
}  // namespace Ion
