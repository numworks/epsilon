#include "apps_container.h"

Escher::App::Snapshot * AppsContainer::initialAppSnapshot() {
  return appSnapshotAtIndex(numberOfApps() == 2 ? 1 : 0);
}
