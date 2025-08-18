#include <apps/apps_container_helper.h>

Shared::GlobalContext* AppsContainerHelper::sharedAppsContainerGlobalContext() {
  return Shared::GlobalContextAccessor::GlobalContext();
}

void AppsContainerHelper::notifyCountryChangeToSnapshots() {
  // No snapshots
}
