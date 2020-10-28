#include "apps_container.h"

Escher::App::Snapshot * AppsContainer::initialAppSnapshot() {
  return onBoardingAppSnapshot();
}
