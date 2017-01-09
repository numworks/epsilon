#include "box_controller.h"
#include <math.h>

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, Store * store) :
  ViewController(parentResponder),
  m_boxBannerView(BoxBannerView()),
  m_view(BoxView(store, &m_boxBannerView)),
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
    if (m_view.selectQuantile(nextSelectedQuantile)) {
      reloadBannerView();
      return true;
    }
    return false;
  }
  return false;
}

void BoxController::didBecomeFirstResponder() {
  m_view.selectMainView(true);
  m_view.reload();
  reloadBannerView();
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

void BoxController::reloadBannerView() {
  const char * calculationName[5] = {"Minimum", "Premier quartile", "Mediane", "Troisieme quartile", "Maximum"};
  m_boxBannerView.setLegendAtIndex((char *)calculationName[m_view.selectedQuantile()], 0);

  char buffer[Constant::FloatBufferSizeInScientificMode];
  float calculation = 0.0f;
  switch(m_view.selectedQuantile()) {
    case 0:
      calculation = m_store->minValue();
      break;
    case 1:
      calculation = m_store->firstQuartile();
      break;
    case 2:
      calculation = m_store->median();
      break;
    case 3:
      calculation = m_store->thirdQuartile();
      break;
    case 4:
      calculation = m_store->maxValue();
      break;
  }
  Float(calculation).convertFloatToText(buffer, Constant::FloatBufferSizeInScientificMode, Constant::NumberOfDigitsInMantissaInScientificMode);
  m_boxBannerView.setLegendAtIndex(buffer, 1);
  m_boxBannerView.layoutSubviews();
}

}
