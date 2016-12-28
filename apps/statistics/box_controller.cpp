#include "box_controller.h"
#include <math.h>

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, Data * data) :
  ViewController(parentResponder),
  m_view(BoxView(data)),
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
    m_view.selectMainView(false);
    app()->setFirstResponder(tabController());
    return true;
  }
  if (event == Ion::Events::Left || event == Ion::Events::Right) {
    int nextSelectedQuantile = event == Ion::Events::Left ? m_view.selectedQuantile()-1 : m_view.selectedQuantile()+1;
    return m_view.selectQuantile(nextSelectedQuantile);
  }
  return false;
}

void BoxController::didBecomeFirstResponder() {
  m_view.selectMainView(true);
  m_view.reload(-1);
}

bool BoxController::isEmpty() {
  if (m_data->totalSize() == 0) {
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
