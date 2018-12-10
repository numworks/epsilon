#include "sandbox_controller.h"
#include <apps/apps_container.h>

namespace Code {

SandboxController::SandboxController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_solidColorView(KDColorWhite)
{
}

StackViewController * SandboxController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

void SandboxController::viewWillAppear() {
  static_cast<AppsContainer *>(const_cast<Container *>(app()->container()))->redrawWindow();
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

}
