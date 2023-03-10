#include "localization_controller.h"

#include <apps/apps_container.h>
#include <apps/global_preferences.h>

#include <algorithm>

using namespace Escher;

namespace OnBoarding {

int LocalizationController::indexOfCellToSelectOnReset() const {
  return mode() == Mode::Language
             ? Shared::LocalizationController::indexOfCellToSelectOnReset()
             : IndexOfCountry(
                   I18n::DefaultCountryForLanguage[static_cast<uint8_t>(
                       GlobalPreferences::sharedGlobalPreferences
                           ->language())]);
}

bool LocalizationController::handleEvent(Ion::Events::Event event) {
  if (Shared::LocalizationController::handleEvent(event)) {
    if (mode() == Mode::Language) {
      setMode(Mode::Country);
      viewWillAppear();
    } else {
      assert(mode() == Mode::Country);
      AppsContainer* appsContainer = AppsContainer::sharedAppsContainer();
      if (appsContainer->promptController()) {
        Container::activeApp()->displayModalViewController(
            appsContainer->promptController(), KDGlyph::k_alignCenter,
            KDGlyph::k_alignCenter);
      } else {
        appsContainer->switchToBuiltinApp(appsContainer->appSnapshotAtIndex(0));
      }
    }
    return true;
  }
  if (event == Ion::Events::Back) {
    if (mode() == Mode::Country) {
      setMode(Mode::Language);
      viewWillAppear();
    }
    return true;
  }
  return false;
}

}  // namespace OnBoarding
