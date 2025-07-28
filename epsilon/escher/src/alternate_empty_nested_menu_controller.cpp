#include <escher/alternate_empty_nested_menu_controller.h>

namespace Escher {

void AlternateEmptyNestedMenuController::viewDidDisappear() {
  if (m_isEmpty) {
    m_isEmpty = false;
    pop();
  }
  NestedMenuController::viewDidDisappear();
}

void AlternateEmptyNestedMenuController::handleResponderChainEvent(
    Responder::ResponderChainEvent event) {
  if (event.type == ResponderChainEventType::HasBecomeFirst) {
    // If the content is empty, we push an empty controller.
    m_isEmpty = (numberOfRows() == 0);
    if (m_isEmpty) {
      push(&m_emptyController);
      return;
    }
    NestedMenuController::handleResponderChainEvent(event);
  } else {
    NestedMenuController::handleResponderChainEvent(event);
  }
}

bool AlternateEmptyNestedMenuController::returnToPreviousMenu() {
  m_isEmpty = false;
  return NestedMenuController::returnToPreviousMenu();
}

}  // namespace Escher
