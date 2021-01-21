#include "shared_app.h"
#include "global_context.h"
#include <apps/apps_container.h>

void SharedApp::Snapshot::pack(App * app) {
  /* Since the sequence store is now accessible from every app, when exiting
   * any application, we need to tidy it.*/
  static_cast<Shared::GlobalContext *>(AppsContainer::sharedAppsContainer()->globalContext())->sequenceStore()->tidy();
  App::Snapshot::pack(app);
}