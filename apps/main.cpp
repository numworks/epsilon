#include "apps_container.h"

AppsContainer container;

void ion_app() {
  container.switchTo(container.appAtIndex(0));
  container.run();
}
