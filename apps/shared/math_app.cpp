#include "math_app.h"

namespace Shared {

bool MathAppWithStoreMenu::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Sto || event == Ion::Events::Var) {
    storeValue();
    return true;
  }
  return MathApp::handleEvent(event);
}

void MathAppWithStoreMenu::storeValue(const char* text) {
  if (m_modalViewController.isDisplayingModal()) {
    return;
  }
  m_storeMenuController.setText(text);
  m_storeMenuController.open();
}

bool MathAppWithStoreMenu::isStoreMenuOpen() const {
  return m_modalViewController.currentModalViewController() ==
         &m_storeMenuController;
}
}  // namespace Shared
