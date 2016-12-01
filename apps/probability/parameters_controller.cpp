#include "parameters_controller.h"

namespace Probability {

ParametersController::ParametersController(Responder * parentResponder, Law * law) :
  ViewController(parentResponder),
  m_view(SolidColorView(KDColor::RGB24(0xF3619B))),
  m_law(law)
{
}

View * ParametersController::view() {
  return &m_view;
}

const char * ParametersController::title() const {
  return "Parameters";
}

bool ParametersController::handleEvent(Ion::Events::Event event) {
  return false;
}

void ParametersController::didResignFirstResponder() {
  m_law->setType(Law::Type::NoType);
}

}

