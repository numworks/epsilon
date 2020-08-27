#include "console_controller.h"

namespace Code {

bool ConsoleController::handleQuitEvent(Ion::Events::Event event) {
  return false;
}

void ConsoleController::popStackViewController(TextField * textField) {
  stackViewController()->pop();
}

}
