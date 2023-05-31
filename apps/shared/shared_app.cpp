#include "shared_app.h"

#include <apps/apps_container_helper.h>

#include "global_context.h"

namespace Shared {

void SharedApp::Snapshot::pack(App* app) {
  /* Since the sequence store and the continuous function store is now
   * accessible from every app, when exiting any application, we need to tidy
   * it.*/
  AppsContainerHelper::sharedAppsContainerGlobalContext()
      ->tidyDownstreamPoolFrom();
  Escher::App::Snapshot::pack(app);
}

void SharedApp::Snapshot::reset() {
  AppsContainerHelper::sharedAppsContainerGlobalContext()->reset();
}

}  // namespace Shared
