#include "calculation_controller.h"

namespace Statistics {

CalculationController::CalculationController(Responder * parentResponder, Data * data) :
  ViewController(parentResponder),
  m_view(SolidColorView(KDColorGreen)),
  m_data(data)
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
  if (m_data->totalSize() == 0) {
    return true;
  }
  return false;
}

const char * CalculationController::emptyMessage() {
  return "Aucune grandeur a calculer";
}

Responder * CalculationController::defaultController() {
  return tabController();
}

Responder * CalculationController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}
