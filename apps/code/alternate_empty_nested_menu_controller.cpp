#include "alternate_empty_nested_menu_controller.h"

void AlternateEmptyNestedMenuController::viewDidDisappear() {
  if (isDisplayingEmptyController()) {
    m_isEmpty = false;
    pop();
  }
  NestedMenuController::viewDidDisappear();
}

bool AlternateEmptyNestedMenuController::displayEmptyControllerIfNeeded() {
  // If the content is empty, we push an empty controller.
  m_isEmpty = (numberOfRows() == 0);
  if (m_isEmpty) {
    push(emptyViewController());
  }
  return m_isEmpty;
}

bool AlternateEmptyNestedMenuController::returnToPreviousMenu() {
  m_isEmpty = false;
  return NestedMenuController::returnToPreviousMenu();
}
