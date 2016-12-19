#include "box_controller.h"

namespace Statistics {

BoxController::BoxController(Responder * parentResponder) :
  ViewController(parentResponder),
  m_view(SolidColorView(KDColorGreen))
{
}

const char * BoxController::title() const {
  return "Boite";
}

View * BoxController::view() {
  return &m_view;
}

bool BoxController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::Up) {
    app()->setFirstResponder(tabController());
    return true;
  }
  return false;
}

bool BoxController::isEmpty() {
  return false;
}

const char * BoxController::emptyMessage() {
  return "Aucune donnée à tracer";
}

Responder * BoxController::defaultController() {
  return tabController();
}

Responder * BoxController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}
