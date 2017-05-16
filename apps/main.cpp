#include "apps_container.h"

AppsContainer container;

void ion_app() {
  container.switchTo(container.onBoardingAppDescriptor());
  container.run();
  container.switchTo(nullptr);
}
