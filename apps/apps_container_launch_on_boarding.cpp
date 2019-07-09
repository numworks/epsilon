#include "apps_container.h"

App::Snapshot * AppsContainer::initialAppSnapshot() {
  return onBoardingAppSnapshot();
}
