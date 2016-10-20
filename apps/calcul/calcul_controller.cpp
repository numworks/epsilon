#include "calcul_controller.h"

namespace Calcul {

CalculController::CalculController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_solidColorView(KDColorBlue)
{
}

View * CalculController::CalculController::view() {
  return &m_solidColorView;
}

const char * CalculController::title() const {
  return "Calcul Table";
}

}
