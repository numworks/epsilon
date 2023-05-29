#include "shared_app.h"

#include <apps/apps_container_helper.h>

#include "../apps_container.h"

using namespace Escher;
using namespace Poincare;

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

SharedApp::SharedApp(Snapshot* snapshot, ViewController* rootViewController)
    : ::App(snapshot, rootViewController, I18n::Message::Warning),
      m_intrusiveStorageChangeFlag(false) {}

Context* SharedApp::localContext() {
  return AppsContainerHelper::sharedAppsContainerGlobalContext();
}

bool SharedAppWithStoreMenu::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Sto || event == Ion::Events::Var) {
    storeValue();
    return true;
  }
  return SharedApp::handleEvent(event);
}

void SharedAppWithStoreMenu::storeValue(const char* text) {
  if (m_modalViewController.isDisplayingModal()) {
    return;
  }
  m_storeMenuController.setText(text);
  m_storeMenuController.open();
}

bool SharedAppWithStoreMenu::isStoreMenuOpen() const {
  return m_modalViewController.currentModalViewController() ==
         &m_storeMenuController;
}

}  // namespace Shared
