#include "apps_container.h"

AppsContainer container;

void ion_app() {
  container.switchTo(container.onBoardingAppSnapshot());
  container.run();
  container.switchTo(nullptr);
}
