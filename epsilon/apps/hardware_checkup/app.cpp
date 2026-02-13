#include "app.h"

#include <apps/apps_container.h>
#include <ion/circuit_breaker.h>
#include <ion/events.h>

using namespace Escher;

namespace HardwareCheckup {

App* App::Snapshot::unpack(Container* container) {
  return new (container->currentAppBuffer()) App(this);
}

constexpr static App::Descriptor sDescriptor;

const App::Descriptor* App::Snapshot::descriptor() const {
  return &sDescriptor;
}

App::App(Snapshot* snapshot)
    : ::App(snapshot, &m_wizardViewController),
      m_wizardViewController(&m_modalViewController) {}

App::WizardViewController::WizardViewController(Responder* parentResponder)
    : BankViewController(parentResponder),
      m_storageViewerController(this),
      m_persistingBytesViewerController(this)
// m_col3Controller(this),
// m_externalAppsController(this),
// m_standByController(this),
// m_screenController(this),
// m_pressedKeyController(this),
// m_otpController(this)
{}

ViewController* App::WizardViewController::childAtIndex(int i) {
  ViewController* children[] = {
      &m_storageViewerController, &m_persistingBytesViewerController,
      // &m_col3Controller,          &m_externalAppsController,
      // &m_standByController,       &m_screenController,
      // &m_pressedKeyController,    &m_otpController
  };
  return children[i];
}

bool App::WizardViewController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Right || event == Ion::Events::Left) {
    setActiveIndex((activeIndex() + numberOfChildren() +
                    (event == Ion::Events::Right ? 1 : -1)) %
                   numberOfChildren());
    return true;
  }
  return false;
}

}  // namespace HardwareCheckup
