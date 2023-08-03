#include <escher/alternate_empty_nested_menu_controller.h>

namespace Escher {

AlternateEmptyNestedMenuController::EmptyNestedMenuController::
    EmptyNestedMenuView::EmptyNestedMenuView(I18n::Message message)
    : ModalViewEmptyController::ModalViewEmptyView() {
  initMessageViews();
  m_message.setMessage(message);
}

void AlternateEmptyNestedMenuController::viewDidDisappear() {
  if (m_isEmpty) {
    m_isEmpty = false;
    pop();
  }
  NestedMenuController::viewDidDisappear();
}

void AlternateEmptyNestedMenuController::didBecomeFirstResponder() {
  // If the content is empty, we push an empty controller.
  m_isEmpty = (numberOfRows() == 0);
  if (m_isEmpty) {
    push(&m_emptyController);
    return;
  }
  NestedMenuController::didBecomeFirstResponder();
}

bool AlternateEmptyNestedMenuController::returnToPreviousMenu() {
  m_isEmpty = false;
  return NestedMenuController::returnToPreviousMenu();
}

}  // namespace Escher
