#include "console_controller.h"

namespace Code {

bool ConsoleController::handleQuitEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Home || event == Ion::Events::Back) {
    if (m_inputRunLoopActive) {
      terminateInputLoop();
    }
    return true;
  }
  return false;
}

void ConsoleController::popStackViewController(TextField * textField) {
  /* In order to lock the console controller, we disable poping controllers
   * below the console controller included. The stack should only hold:
   * - the menu controller
   * - the console controller
   * The depth of the stack controller must always be above or equal to 2. */
  if (stackViewController()->depth() > 2) {
    stackViewController()->pop();
  } else {
    textField->setEditing(true);
  }
}

}
