#include "apps_container.h"

Escher::App::Snapshot * AppsContainer::initialAppSnapshot() {
  return appSnapshotAtIndex(numberOfBuiltinApps() == 2 ? 1 : 0);
}
