#ifndef PROBABILITY_PARAMETERS_CONTROLLER_H
#define PROBABILITY_PARAMETERS_CONTROLLER_H

#include <escher.h>

namespace Probability {

class ParametersController : public ViewController {
public:
  ParametersController(Responder * parentResponder);

  View * view() override;
  const char * title() const override;
  bool handleEvent(ion_event_t event) override;
private:
  SolidColorView m_view;
};

}

#endif
