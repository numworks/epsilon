#include "omega_controller.h"
#include <assert.h>

namespace Omega {

OmegaController::OmegaController(Responder * parentResponder) :
  ViewController(parentResponder)
{
}

View * OmegaController::view() {
  return &m_omegaView;
}

void OmegaController::didBecomeFirstResponder() {
}

bool OmegaController::handleEvent(Ion::Events::Event event) {
  /* if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    return true;
  } */
  return false;
}

}
