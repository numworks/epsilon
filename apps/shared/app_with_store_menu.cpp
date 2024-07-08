#include "app_with_store_menu.h"

using namespace Escher;
using namespace Poincare;

namespace Shared {

bool AppWithStoreMenu::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Sto || event == Ion::Events::Var) {
    storeValue();
    return true;
  }
  return SharedApp::handleEvent(event);
}

void AppWithStoreMenu::storeValue(const char* text) {
  if (m_modalViewController.isDisplayingModal()) {
    return;
  }
  m_storeMenuController.setText(text);
  m_storeMenuController.open();
}

void AppWithStoreMenu::storeLayout(Poincare::Layout layout) {
  if (m_modalViewController.isDisplayingModal()) {
    return;
  }
  m_storeMenuController.setLayout(layout);
  m_storeMenuController.open();
}

bool AppWithStoreMenu::isStoreMenuOpen() const {
  return m_modalViewController.currentModalViewController() ==
         &m_storeMenuController;
}

AppWithStoreMenu::AppWithStoreMenu(SharedApp::Snapshot* snapshot,
                                   Escher::ViewController* rootViewController)
    : SharedApp(snapshot, rootViewController),
      m_intrusiveStorageChangeFlag(false) {}

}  // namespace Shared
