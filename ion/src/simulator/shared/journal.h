#ifndef ION_SIMULATOR_JOURNAL_H
#define ION_SIMULATOR_JOURNAL_H

#include <ion/events.h>

namespace Ion {
namespace Simulator {
namespace Journal {

void init();

Ion::Events::Journal* replayJournal();
Ion::Events::Journal* logJournal();

}  // namespace Journal
}  // namespace Simulator
}  // namespace Ion

#endif
