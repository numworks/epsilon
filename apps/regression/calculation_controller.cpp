#include "calculation_controller.h"

namespace Regression {

CalculationController::CalculationController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(SolidColorView(KDColorGreen))
{
}

const char * CalculationController::title() const {
  return "Stats";
}

View * CalculationController::view() {
  return &m_view;
}

bool CalculationController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    app()->setFirstResponder(tabController());
    return true;
  }
  return false;
}

bool CalculationController::isEmpty() {
  return false;
}

const char * CalculationController::emptyMessage() {
  return "Aucune donnée à tracer";
}

Responder * CalculationController::defaultController() {
  return tabController();
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}

