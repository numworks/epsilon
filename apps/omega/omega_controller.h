#ifndef APPS_OMEGA_CONTROLLER_H
#define APPS_OMEGA_CONTROLLER_H

#include <escher.h>
#include "omega_view.h"

namespace Omega {

class OmegaController : public ViewController {
public:
  OmegaController(Responder * parentResponder);
  View * view() override;
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
private:
  OmegaView m_omegaView;
};

}

#endif
