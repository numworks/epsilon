#include "menu_controller.h"

namespace Code {

void MenuController::bypassIfLocked() {
  consoleController()->setAutoImport(true);
  stackViewController()->push(consoleController());
}

}
