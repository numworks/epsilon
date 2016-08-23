#include "parameters_controller.h"

Probability::ParametersController::ParametersController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(SolidColorView(KDColor(0xF3619B)))
{
}

View * Probability::ParametersController::view() {
  return &m_view;
}

const char * Probability::ParametersController::title() const {
  return "Parameters";
}

bool Probability::ParametersController::handleEvent(Ion::Events::Event event) {
  return false;
}
