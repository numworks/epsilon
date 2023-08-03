#ifndef ESCHER_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H
#define ESCHER_ALTERNATE_EMPTY_NESTED_MENU_CONTROLLER_H

#include "modal_view_empty_controller.h"
#include "nested_menu_controller.h"

namespace Escher {

class AlternateEmptyNestedMenuController : public NestedMenuController {
 public:
  AlternateEmptyNestedMenuController(I18n::Message title,
                                     I18n::Message emptyMessage)
      : NestedMenuController(nullptr, title),
        m_emptyController(emptyMessage),
        m_isEmpty(false) {}
  void viewDidDisappear() override;

 protected:
  void didBecomeFirstResponder() override;
  bool returnToPreviousMenu() override;

 private:
  ModalViewEmptyController m_emptyController;
  bool m_isEmpty;
};

}  // namespace Escher

#endif
