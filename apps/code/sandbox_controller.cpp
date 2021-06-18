#include "sandbox_controller.h"
#include <apps/apps_container.h>

extern "C" {
#include <python/port/mod/turtle/modturtle.h>
}

namespace Code {

SandboxController::SandboxController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_solidColorView(Palette::CodeBackground)
{
}

StackViewController * SandboxController::stackViewController() {
  return static_cast<StackViewController *>(parentResponder());
}

void SandboxController::reset() {
  m_solidColorView.reload();
  redrawWindow();
}

void SandboxController::viewWillAppear() {
  redrawWindow();
}

void SandboxController::viewDidDisappear() {
  modturtle_view_did_disappear();
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
