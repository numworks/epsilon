#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/global_preferences.h>

Shared::GlobalContext* AppsContainerHelper::sharedAppsContainerGlobalContext() {
  return AppsContainer::sharedAppsContainer()->globalContext();
}

void AppsContainerHelper::notifyCountryChangeToSnapshots() {
  int nApps = AppsContainer::sharedAppsContainer()->numberOfBuiltinApps();
  for (int i = 0; i < nApps; i++) {
    AppsContainer::sharedAppsContainer()
        ->appSnapshotAtIndex(i)
        ->countryWasUpdated();
  }
}
