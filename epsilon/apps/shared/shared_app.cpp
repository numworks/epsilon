#include "shared_app.h"

#include <apps/apps_container_helper.h>
#include <apps/shared/global_store.h>
#include <poincare/symbol_context.h>

#include "../apps_container.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

/* TODO: ContinuousFunctionStore and SequenceStore should be removed from global
 * context and go in their respective apps. For now, SequenceStore does not need
 * to be global, and ContinuousFunctionStore needs to be global only because
 * it's costly to recompute the function symbol and layout each time the varbox
 * is opened.
 * Once this is done, these two methods can probably be delete. */
void SharedApp::Snapshot::tidy() {
  /* Since the sequence store and the continuous function store is now
   * accessible from every app, when exiting any application, we need to tidy
   * it.*/
  GlobalContextAccessor::Store().tidyStores();
  Escher::App::Snapshot::tidy();
}

void SharedApp::Snapshot::reset() { GlobalContextAccessor::Store().reset(); }

SharedApp::SharedApp(Snapshot* snapshot, ViewController* rootViewController)
    : ::App(snapshot, rootViewController) {}

const Poincare::SymbolContext& SharedApp::localContext() const {
  return GlobalContextAccessor::Context();
}

}  // namespace Shared
