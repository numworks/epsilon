#ifndef SHARED_MATH_APP_H
#define SHARED_MATH_APP_H

#include "app_with_store_menu.h"
#include "math_toolbox.h"
#include "math_variable_box.h"

namespace Shared {

class MathApp : public AppWithStoreMenu {
 public:
  MathToolbox* defaultToolbox() override final { return &m_mathToolbox; }
  MathVariableBox* defaultVariableBox() override final {
    return &m_mathVariableBox;
  }

 protected:
  using AppWithStoreMenu::AppWithStoreMenu;

 private:
  MathToolbox m_mathToolbox;
  MathVariableBox m_mathVariableBox;
};

}  // namespace Shared
#endif
