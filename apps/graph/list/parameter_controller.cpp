#include "parameter_controller.h"

ParameterController::ParameterController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_solidColorView(SolidColorView(KDColorRed))
{
}

View * ParameterController::view() {
  return &m_solidColorView;
}

const char * ParameterController::title() const {
  return "List Function Parameter";
}

bool ParameterController::handleEvent(Ion::Events::Event event) {
  return false;
}
