#include "apps_container.h"

#include <stdlib.h>

AppsContainer container;

const char * apps_list = APPS_LIST;
const int apps_list_length = strlen(apps_list)+1;

void ion_app() {
  /* Dynamically allocate a copy of apps list instead of permanently wasting
   * some RAM. */
  char *apps_list_work = (char*)malloc(apps_list_length);
  memcpy(apps_list_work, apps_list, apps_list_length);

  char *saveptr;
  char *tok = strtok_r(apps_list_work, " ", &saveptr);

  /* Register app snapshots at run-time in order of declaration in APPS_LIST.
   * This is a rather naive implementation, but it is called only once when
   * the calculator cold boots and we don't expect to handle hundreds of apps
   * inside a single calculator for now. */
  while (tok) {
    App::Snapshot::Register *r = App::Snapshot::Register::getList();
    assert(r);

    while (strcmp(tok, r->getSnapshot()->descriptor()->uriName())) {
      r = r->getNext();
      assert(r);
    }

    container.registerAppSnapshot(r->getSnapshot());
    tok = strtok_r(nullptr, " ", &saveptr);
  }

  free(apps_list_work);

  container.run();
}

