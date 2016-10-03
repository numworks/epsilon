#include "apps_container.h"

void ion_app() {
  AppsContainer container;
  container.switchTo(container.appAtIndex(0));
  container.run();
}
