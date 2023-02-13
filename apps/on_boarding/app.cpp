#include "app.h"

#include <assert.h>

#include "../apps_container.h"

using namespace Escher;

namespace OnBoarding {

App* App::Snapshot::unpack(Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot* snapshot)
    : ::App(snapshot, &m_localizationController),
      m_localizationController(&m_modalViewController,
                               LocalizationController::Mode::Language) {}

void App::willBecomeInactive() {
  Ion::Power::selectStandbyMode(false);
  Ion::Events::setSpinner(true);
  ::App::willBecomeInactive();
}

void App::didBecomeActive(Window* window) {
  ::App::didBecomeActive(window);
  // Disable spinner
  Ion::Events::setSpinner(false);
  if (Poincare::Preferences::sharedPreferences->examMode().color() ==
      KDColorBlack) {
    // Forbid standby in exam mode with led since it disables the led
    Ion::Power::selectStandbyMode(true);
  }
}

}  // namespace OnBoarding
