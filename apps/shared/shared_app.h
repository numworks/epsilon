#ifndef SHARED_APP_H
#define SHARED_APP_H

#include <escher/app.h>
#include <escher/boxes_delegate.h>
#include <escher/nested_menu_controller.h>
#include <escher/pervasive_box.h>

#include "store_menu_controller.h"

namespace Shared {

class SharedApp : public Escher::App, public Escher::BoxesDelegate {
 public:
  class Snapshot : public App::Snapshot {
   public:
    void tidy() override;
    void reset() override;
  };
  virtual ~SharedApp() = default;
  Poincare::Context* localContext() override;

  virtual void prepareForIntrusiveStorageChange() {
    assert(!m_intrusiveStorageChangeFlag);
    m_intrusiveStorageChangeFlag = true;
  }
  virtual void concludeIntrusiveStorageChange() {
    assert(m_intrusiveStorageChangeFlag);
    m_intrusiveStorageChangeFlag = false;
  }

 protected:
  SharedApp(Snapshot* snapshot, Escher::ViewController* rootViewController);

  bool m_intrusiveStorageChangeFlag;
};

class SharedAppWithStoreMenu : public SharedApp {
 public:
  virtual ~SharedAppWithStoreMenu() = default;
  void storeValue(const char* text = "") override;
  bool isStoreMenuOpen() const;

 protected:
  using SharedApp::SharedApp;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  StoreMenuController m_storeMenuController;
};

}  // namespace Shared
#endif
