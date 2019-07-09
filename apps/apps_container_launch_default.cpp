#include "apps_container.h"

App::Snapshot * AppsContainer::initialAppSnapshot() {
  // The backlight has not been initialized
  Ion::Backlight::init();
  return appSnapshotAtIndex(numberOfApps() == 2 ? 1 : 0);
}
