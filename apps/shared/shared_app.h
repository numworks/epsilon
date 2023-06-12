#ifndef SHARED_APP_H
#define SHARED_APP_H

#include <escher/app.h>
#include <escher/nested_menu_controller.h>
#include <escher/pervasive_box.h>

#include "store_menu_controller.h"

namespace Shared {

class SharedApp : public Escher::App {
 public:
  class Snapshot : public Escher::App::Snapshot {
   public:
    void tidy() override;
    void reset() override;
  };
  Poincare::Context* localContext() override;

 protected:
  SharedApp(Snapshot* snapshot, Escher::ViewController* rootViewController);
};

class SharedAppWithStoreMenu : public SharedApp {
 public:
  void storeValue(const char* text = "") override;
  bool isStoreMenuOpen() const;
  Escher::PervasiveBox* toolbox() override final {
    return isStoreMenuOpen() ? nullptr : defaultToolbox();
  }
  Escher::PervasiveBox* variableBox() override final {
    return isStoreMenuOpen() ? nullptr : defaultVariableBox();
  }
  virtual Escher::PervasiveBox* defaultToolbox() { return nullptr; }
  virtual Escher::PervasiveBox* defaultVariableBox() { return nullptr; }

  virtual void prepareForIntrusiveStorageChange() {
    assert(!m_intrusiveStorageChangeFlag);
    m_intrusiveStorageChangeFlag = true;
  }
  virtual void concludeIntrusiveStorageChange() {
    assert(m_intrusiveStorageChangeFlag);
    m_intrusiveStorageChangeFlag = false;
  }

 protected:
  SharedAppWithStoreMenu(SharedApp::Snapshot* snapshot,
                         Escher::ViewController* rootViewController);
  bool handleEvent(Ion::Events::Event event) override;

 private:
  StoreMenuController m_storeMenuController;

 protected:
  bool m_intrusiveStorageChangeFlag;
};

}  // namespace Shared
#endif
