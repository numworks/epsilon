#include <apps/apps_container.h>
#include <apps/apps_container_helper.h>
#include <apps/global_preferences.h>

Shared::GlobalContext* AppsContainerHelper::sharedAppsContainerGlobalContext() {
  return AppsContainer::sharedAppsContainer()->globalContext();
}
