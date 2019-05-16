#include "app.h"
#include "../apps_container.h"

extern "C" {
#include <assert.h>
}

namespace HardwareTest {

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Snapshot * snapshot) :
  ::App(snapshot, &m_wizardViewController),
  m_wizardViewController(&m_modalViewController)
{
}

App::WizardViewController::WizardViewController(Responder * parentResponder) :
  BankViewController(parentResponder),
  m_keyboardController(this),
  m_screenTestController(this),
  m_ledTestController(this),
  m_batteryTestController(this),
  m_serialNumberController(this)
{
}

int App::WizardViewController::numberOfChildren() {
  return 5;
}

ViewController * App::WizardViewController::childAtIndex(int i) {
  ViewController * children[] = {
    &m_keyboardController,
    &m_screenTestController,
    &m_ledTestController,
    &m_batteryTestController,
    &m_serialNumberController
  };
  return children[i];
}

bool App::WizardViewController::handleEvent(Ion::Events::Event event) {
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
