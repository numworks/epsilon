#include "box_controller.h"
#include <math.h>

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, Store * store) :
  ViewController(parentResponder),
  m_view(BoxView(store)),
  m_store(store)
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
  uint32_t storeChecksum = m_store->checksum();
  if (m_storeVersion != storeChecksum) {
    m_storeVersion = storeChecksum;
    m_store->initWindowParameters();
    m_store->initBarParameters();
  }
  m_view.selectMainView(true);
  m_view.reload();
}

bool BoxController::isEmpty() {
  if (m_store->sumOfColumn(1) == 0) {
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
