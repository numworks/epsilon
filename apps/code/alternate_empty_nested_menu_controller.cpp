#include "alternate_empty_nested_menu_controller.h"

void AlternateEmptyNestedMenuController::viewDidDisappear() {
  if (isDisplayingEmptyController()) {
    m_isEmpty = false;
    pop();
  }
  NestedMenuController::viewDidDisappear();
}

void AlternateEmptyNestedMenuController::didBecomeFirstResponder() {
  // If the content is empty, we push an empty controller.
  m_isEmpty = (numberOfRows() == 0);
  if (m_isEmpty) {
    push(emptyViewController());
    return;
  }
  Escher::NestedMenuController::didBecomeFirstResponder();
}

bool AlternateEmptyNestedMenuController::returnToPreviousMenu() {
  m_isEmpty = false;
  return NestedMenuController::returnToPreviousMenu();
}
