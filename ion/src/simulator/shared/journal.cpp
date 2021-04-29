#include "journal.h"
#include "journal/queue_journal.h"
#include <queue>

namespace Ion {
namespace Simulator {
namespace Journal {

void init() {
  Events::logTo(logJournal());
}

Events::Journal * replayJournal() {
  static QueueJournal journal;
  return &journal;
}

Events::Journal * logJournal() {
  static QueueJournal journal;
  return &journal;
}

}
}
}
