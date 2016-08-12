#include "apps_container.h"

void ion_app() {
  AppsContainer container;
  container.switchTo(AppsContainer::AppId::Graph);
  container.run();
}
