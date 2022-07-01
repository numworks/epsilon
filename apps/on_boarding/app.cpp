#include "app.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Escher;

namespace OnBoarding {

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_localizationController),
  m_localizationController(&m_modalViewController, LocalizationController::Mode::Language)
{
}

void App::willBecomeInactive() {
  Ion::Power::selectStandbyMode(false);
  Ion::Events::setSpinner(true);
  ::App::willBecomeInactive();
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  // Disable spinner
  Ion::Events::setSpinner(false);
  // Force a core reset to exit
  Ion::Power::selectStandbyMode(true);
}

}
