#include "localization_controller.h"
#include <apps/global_preferences.h>


namespace Settings {

int LocalizationController::indexOfCellToSelectOnReset() const {
  return mode() == Mode::Language ?
    static_cast<int>(GlobalPreferences::sharedGlobalPreferences()->language()) :
    IndexOfCountry(GlobalPreferences::sharedGlobalPreferences()->country());
}

bool LocalizationController::handleEvent(Ion::Events::Event event) {
  if (Shared::LocalizationController::handleEvent(event) || event == Ion::Events::Left) {
    static_cast<StackViewController *>(parentResponder())->pop();
    return true;
  }
  return false;
}

}
