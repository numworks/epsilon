#ifndef SHARED_MATH_APP_H
#define SHARED_MATH_APP_H

#include <apps/math_toolbox.h>
#include <apps/math_variable_box_controller.h>

#include "shared_app.h"
#include "store_menu_controller.h"

namespace Shared {

class MathApp : public SharedApp {
 public:
  virtual ~MathApp() = default;
  MathToolbox* toolbox() override { return &m_mathToolbox; }
  MathVariableBoxController* variableBox() override {
    return &m_variableBoxController;
  }

 protected:
  using SharedApp::SharedApp;

 private:
  MathToolbox m_mathToolbox;
  MathVariableBoxController m_variableBoxController;
};

class MathAppWithStoreMenu : public MathApp {
 public:
  virtual ~MathAppWithStoreMenu() = default;
  void storeValue(const char* text = "") override;
  bool isStoreMenuOpen() const;

 protected:
  using MathApp::MathApp;
  bool handleEvent(Ion::Events::Event event) override;

 private:
  StoreMenuController m_storeMenuController;
};

}  // namespace Shared
#endif
