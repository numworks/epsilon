#include "box_controller.h"

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, Data * data) :
  ViewController(parentResponder),
  m_view(SolidColorView(KDColorGreen)),
  m_data(data)
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
  if (m_data->numberOfPairs() == 0) {
    return true;
  }
  return false;
}
const char * BoxController::emptyMessage() {
  return "Aucune donnee a tracer";
}

Responder * BoxController::defaultController() {
  return tabController();
}

Responder * BoxController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

}
