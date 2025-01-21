#ifndef SHARED_APP_WITH_STORE_MENU_H
#define SHARED_APP_WITH_STORE_MENU_H

#include "shared_app.h"
#include "store_menu_controller.h"

namespace Shared {

class AppWithStoreMenu : public SharedApp {
 public:
  bool canStoreLayout() override { return true; }
  void storeLayout(Poincare::Layout layout = {}) override;
  bool isStoreMenuOpen() const;
  Escher::EditableFieldHelpBox* toolbox() override final {
    return isStoreMenuOpen() ? nullptr : defaultToolbox();
  }
  Escher::EditableFieldHelpBox* variableBox() override final {
    return isStoreMenuOpen() ? nullptr : defaultVariableBox();
  }
  virtual Escher::EditableFieldHelpBox* defaultToolbox() { return nullptr; }
  virtual Escher::EditableFieldHelpBox* defaultVariableBox() { return nullptr; }

  virtual void prepareForIntrusiveStorageChange() {
    assert(!m_intrusiveStorageChangeFlag);
    m_intrusiveStorageChangeFlag = true;
  }
  virtual void concludeIntrusiveStorageChange() {
    assert(m_intrusiveStorageChangeFlag);
    m_intrusiveStorageChangeFlag = false;
  }

 protected:
  AppWithStoreMenu(SharedApp::Snapshot* snapshot,
                   Escher::ViewController* rootViewController);
  bool handleEvent(Ion::Events::Event event) override;

 private:
  StoreMenuController m_storeMenuController;

 protected:
  bool m_intrusiveStorageChangeFlag;
};

}  // namespace Shared
#endif
