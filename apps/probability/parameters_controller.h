#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>
#include "law.h"

namespace Probability {

class ParametersController : public ViewController {
public:
  ParametersController(Responder * parentResponder, Law * law);

  View * view() override;
  const char * title() const override;
  bool handleEvent(Ion::Events::Event event) override;
  void didResignFirstResponder() override;
private:
  SolidColorView m_view;
  Law * m_law;
};

}

#endif
