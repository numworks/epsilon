#include <apps/global_preferences.h>
#include <apps/apps_container_helper.h>
#include <apps/apps_container.h>

Shared::GlobalContext * AppsContainerHelper::sharedAppsContainerGlobalContext() {
  return AppsContainer::sharedAppsContainer()->globalContext();
}
