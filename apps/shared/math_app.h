#ifndef SHARED_MATH_APP_H
#define SHARED_MATH_APP_H

#include <apps/math_toolbox.h>
#include <apps/math_variable_box_controller.h>

#include "shared_app.h"

namespace Shared {

class MathApp : public SharedAppWithStoreMenu {
 public:
  virtual ~MathApp() = default;
  MathToolbox* toolbox() override { return &m_mathToolbox; }
  MathVariableBoxController* variableBox() override {
    return &m_variableBoxController;
  }

 protected:
  using SharedAppWithStoreMenu::SharedAppWithStoreMenu;

 private:
  MathToolbox m_mathToolbox;
  MathVariableBoxController m_variableBoxController;
};

}  // namespace Shared
#endif
