#include "app.h"
#include "../apps_container.h"

namespace HardwareTest {

App * App::Snapshot::unpack(Container * container) {
  return new (container->currentAppBuffer()) App(container, this);
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_wizardViewController),
  m_wizardViewController(&m_modalViewController)
{
}

App::WizardViewController::WizardViewController(Responder * parentResponder) :
  BankViewController(parentResponder),
  m_batteryTestController(this),
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
  return 8;
}

ViewController * App::WizardViewController::childAtIndex(int i) {
  ViewController * children[] = {
    &m_vBlankTestController,
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
