#include "apps_container.h"

Escher::App::Snapshot * AppsContainer::initialAppSnapshot() {
#if EPSILON_GETOPT
  if (m_initialAppSnapshot != nullptr) {
    return m_initialAppSnapshot;
  }
#endif
  return appSnapshotAtIndex(0);
}
