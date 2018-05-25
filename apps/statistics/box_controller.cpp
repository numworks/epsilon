#include "box_controller.h"
#include "app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, ButtonRowController * header, Store * store, BoxView::Quantile * selectedQuantile) :
  MultipleDataViewController(parentResponder, store, (int *)(selectedQuantile)),
  ButtonRowDelegate(header, nullptr),
  m_view(store, selectedQuantile)
{
}

bool BoxController::moveSelectionHorizontally(int deltaIndex) {
  int selectedQuantile = (int)m_view.dataViewAtIndex(selectedSeries())->selectedQuantile();
  int nextSelectedQuantile = selectedQuantile + deltaIndex;
  if (m_view.dataViewAtIndex(selectedSeries())->selectQuantile(nextSelectedQuantile)) {
    reloadBannerView();
    return true;
  }
  return false;
}

const char * BoxController::title() {
  return I18n::translate(I18n::Message::BoxTab);
}

Responder * BoxController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

void BoxController::reloadBannerView() {
  if (selectedSeries() < 0) {
    return;
  }

  int selectedQuantile = (int)m_view.dataViewAtIndex(selectedSeries())->selectedQuantile();

  // Set calculation name
  I18n::Message calculationName[5] = {I18n::Message::Minimum, I18n::Message::FirstQuartile, I18n::Message::Median, I18n::Message::ThirdQuartile, I18n::Message::Maximum};
  m_view.editableBannerView()->setMessageAtIndex(calculationName[selectedQuantile], 0);

  // Set calculation result
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  CalculPointer calculationMethods[5] = {&Store::minValue, &Store::firstQuartile, &Store::median, &Store::thirdQuartile,
    &Store::maxValue};
  double calculation = (m_store->*calculationMethods[selectedQuantile])(selectedSeries());
  PrintFloat::convertFloatToText<double>(calculation, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_view.editableBannerView()->setLegendAtIndex(buffer, 1);
}

}
