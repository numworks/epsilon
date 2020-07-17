#include "localization_controller.h"

namespace Settings {

bool LocalizationController::handleEvent(Ion::Events::Event event) {
  if (Shared::LocalizationController::handleEvent(event) || event == Ion::Events::Left) {
    static_cast<StackViewController *>(parentResponder())->pop();
    return true;
  }
  return false;
}

}
