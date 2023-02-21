#include "shared_app.h"

#include <apps/apps_container_helper.h>

#include "global_context.h"

namespace Shared {

void SharedApp::Snapshot::pack(App* app) {
  /* Since the sequence store is now accessible from every app, when exiting
   * any application, we need to tidy it.*/
  AppsContainerHelper::sharedAppsContainerGlobalContext()
      ->tidyDownstreamPoolFrom();
  App::Snapshot::pack(app);
}

}  // namespace Shared
