#include "app.h"
#include "../apps_container.h"
#include <ion/circuit_breaker.h>
#include <ion/events.h>

using namespace Escher;

namespace HardwareTest {

App * App::Snapshot::unpack(Container * container) {
  // Spinner may break LCD data test
  Ion::Events::setSpinner(false);
  // Disable Home button, so that the Home key can be tested
  Ion::CircuitBreaker::unsetCheckpoint(Ion::CircuitBreaker::CheckpointType::Home);
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor * App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_wizardViewController),
  m_wizardViewController(&m_modalViewController)
{
}

App::WizardViewController::WizardViewController(Responder * parentResponder) :
  BankViewController(parentResponder),
  m_batteryTestController(this),
  m_lcdTimingTestController(this),
  m_colorsLCDTestController(this),
  m_deadPixelsTestController(this),
  m_keyboardController(this),
  m_lcdDataTestController(this),
  m_ledTestController(this),
  m_serialNumberController(this),
  m_vBlankTestController(this)
{
}

int App::WizardViewController::numberOfChildren() {
  return 9;
}

ViewController * App::WizardViewController::childAtIndex(int i) {
  ViewController * children[] = {
    &m_vBlankTestController,
    &m_lcdTimingTestController,
    &m_colorsLCDTestController,
    &m_lcdDataTestController,
    &m_deadPixelsTestController,
    &m_ledTestController,
    &m_keyboardController,
    &m_batteryTestController,
    &m_serialNumberController
  };
  return children[i];
}

bool App::WizardViewController::handleEvent(Ion::Events::Event & event) {
  // Do not handle OnOff event to let the apps container redraw the screen
  if (event == Ion::Events::OnOff) {
    return false;
  }
  if (activeIndex() >= numberOfChildren()) {
    return false;
  }
  setActiveIndex(activeIndex() + 1);
  return true;
}

}
