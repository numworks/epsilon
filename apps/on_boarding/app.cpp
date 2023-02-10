#include "app.h"

#include <assert.h>

#include "../apps_container.h"
#include "apps/exam_mode_configuration.h"

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
  // Force a core reset to exit
  if (!(Poincare::Preferences::sharedPreferences->examMode().isActive() &&
        ExamModeConfiguration::examModeColor(
            Poincare::Preferences::sharedPreferences->examMode()) !=
            KDColorBlack)) {
    // Forbid standby in exam mode with led since it disables the led
    Ion::Power::selectStandbyMode(true);
  }
}

}  // namespace OnBoarding
