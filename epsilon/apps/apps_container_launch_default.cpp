#include "apps_container.h"

Escher::App::Snapshot* AppsContainer::initialAppSnapshot() {
#if EPSILON_GETOPT
  if (m_initialAppSnapshot != nullptr) {
    return m_initialAppSnapshot;
  }
#endif
  assert(homeAppSnapshot() == appSnapshotAtIndex(0));
  return homeAppSnapshot();
}
