#include "apps_container.h"

AppsContainer container;

void ion_app() {
  App::Snapshot::Register *r = App::Snapshot::Register::getList();
  while (r) {
    container.registerAppSnapshot(r->getSnapshot());
    r = r->getNext();
  }

  container.run();
}
