#include "apps_container.h"

AppsContainer container;

void ion_app() {
  container.switchTo(container.appAtIndex(-1));
  container.run();
  container.switchTo(nullptr);
}
