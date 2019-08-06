#include "sandbox_controller.h"
#include <apps/apps_container.h>

namespace Code {

SandboxController::SandboxController(Responder * parentResponder, MicroPython::ExecutionEnvironment * executionEnvironment) :
  ViewController(parentResponder),
  m_solidColorView(KDColorWhite),
  m_executionEnvironment(executionEnvironment)
{
  assert(executionEnvironment != nullptr);
}

StackViewController * SandboxController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

void SandboxController::reset() {
  m_solidColorView.reload();
  redrawWindow();
}

void SandboxController::hide() {
  stackViewController()->pop();
}

void SandboxController::viewWillAppear() {
  assert(m_executionEnvironment != nullptr);
  m_executionEnvironment->setSandboxIsDisplayed(true);
  redrawWindow();
}

void SandboxController::viewDidDisappear() {
  assert(m_executionEnvironment != nullptr);
  m_executionEnvironment->setSandboxIsDisplayed(false);
}

bool SandboxController::handleEvent(Ion::Events::Event event) {
  // The sandbox handles or "absorbs" all keyboard events except Home and OnOff
  if (event == Ion::Events::Home || event == Ion::Events::OnOff) {
    stackViewController()->pop();
    return false;
  }
  if (event == Ion::Events::OK || event == Ion::Events::Back) {
    stackViewController()->pop();
  }
  return event.isKeyboardEvent();
}

void SandboxController::redrawWindow() {
  AppsContainer::sharedAppsContainer()->redrawWindow();
}

}
