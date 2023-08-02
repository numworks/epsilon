#ifndef ESCHER_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H
#define ESCHER_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H

#include "nested_menu_controller.h"

namespace Escher {

class AlternateEmptyNestedMenuController : public NestedMenuController {
 public:
  AlternateEmptyNestedMenuController(I18n::Message title)
      : NestedMenuController(nullptr, title), m_isEmpty(false) {}
  void viewDidDisappear() override;

 protected:
  virtual ViewController* emptyViewController() = 0;
  void didBecomeFirstResponder() override;
  bool returnToPreviousMenu() override;

 private:
  bool m_isEmpty;
};

}  // namespace Escher

#endif
