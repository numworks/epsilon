#include "app.h"
#include "../apps_container.h"
#include <assert.h>

using namespace Escher;

namespace OnBoarding {

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

static constexpr App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_localizationController),
  m_localizationController(&m_modalViewController, LocalizationController::Mode::Language),
  m_logoController()
{
}

int App::numberOfTimers() {
  return firstResponder() == &m_logoController;
}

Timer * App::timerAtIndex(int i) {
  assert(i == 0);
  return &m_logoController;
}

bool App::processEvent(Ion::Events::Event e) {
  if (e == Ion::Events::Home) {
    return true;
  }
  if (e == Ion::Events::OnOff) {
    Ion::Power::standby(); // Force a core reset to exit
  }
  return ::App::processEvent(e);
}

void App::didBecomeActive(Window * window) {
  ::App::didBecomeActive(window);
  reinitOnBoarding();
}

void App::reinitOnBoarding() {
  m_localizationController.resetSelection();
  displayModalViewController(&m_logoController, 0.5f, 0.5f);
}

}
