#include "box_controller.h"
#include "app.h"
#include "../apps_container.h"

using namespace Poincare;

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, ButtonRowController * header, Store * store, BoxView::Quantile * selectedQuantile) :
  ViewController(parentResponder),
  ButtonRowDelegate(header, nullptr),
  m_boxBannerView(),
  m_view(store, &m_boxBannerView, selectedQuantile),
  m_store(store)
{
}

const char * BoxController::title() {
  return I18n::translate(I18n::Message::BoxTab);
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
    int nextSelectedQuantile = event == Ion::Events::Left ? (int)m_view.selectedQuantile()-1 : (int)m_view.selectedQuantile()+1;
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
}

bool BoxController::isEmpty() const {
  if (m_store->sumOfColumn(1) == 0) {
    return true;
  }
  return false;
}

I18n::Message BoxController::emptyMessage() {
  return I18n::Message::NoDataToPlot;
}

Responder * BoxController::defaultController() {
  return tabController();
}

Responder * BoxController::tabController() const {
  return (parentResponder()->parentResponder()->parentResponder());
}

void BoxController::reloadBannerView() {
  I18n::Message calculationName[5] = {I18n::Message::Minimum, I18n::Message::FirstQuartile, I18n::Message::Median, I18n::Message::ThirdQuartile, I18n::Message::Maximum};
  m_boxBannerView.setMessageAtIndex(calculationName[(int)m_view.selectedQuantile()], 0);
  char buffer[PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  CalculPointer calculationMethods[5] = {&Store::minValue, &Store::firstQuartile, &Store::median, &Store::thirdQuartile,
    &Store::maxValue};
  double calculation = (m_store->*calculationMethods[(int)m_view.selectedQuantile()])();
  PrintFloat::convertFloatToText<double>(calculation, buffer, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits);
  m_boxBannerView.setLegendAtIndex(buffer, 1);
}

void BoxController::viewWillAppear() {
  m_view.selectMainView(true);
  reloadBannerView();
  m_view.reload();
}

void BoxController::willExitResponderChain(Responder * nextFirstResponder) {
  if (nextFirstResponder == tabController()) {
    m_view.selectMainView(false);
    m_view.reload();
  }
}

}
