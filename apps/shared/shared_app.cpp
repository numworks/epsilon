#include "shared_app.h"

#include <apps/apps_container_helper.h>

#include "global_context.h"

namespace Shared {

/* TODO: SharedApp will be refactored in v22. This will be a good occasion to
 * put ContinuousFunctionStore and SequenceStore out of the global context and
 * in the MathApp instead. These two methods will then need to be also
 * refactored. */
void SharedApp::Snapshot::tidy() {
  /* Since the sequence store and the continuous function store is now
   * accessible from every app, when exiting any application, we need to tidy
   * it.*/
  AppsContainerHelper::sharedAppsContainerGlobalContext()
      ->tidyDownstreamPoolFrom();
  Escher::App::Snapshot::tidy();
}

void SharedApp::Snapshot::reset() {
  AppsContainerHelper::sharedAppsContainerGlobalContext()->reset();
}

}  // namespace Shared
