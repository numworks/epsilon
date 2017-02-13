#include "box_controller.h"
#include "../apps_container.h"
#include "app.h"
#include <math.h>

using namespace Poincare;

namespace Statistics {

BoxController::BoxController(Responder * parentResponder, HeaderViewController * headerViewController, Store * store) :
  ViewController(parentResponder),
  HeaderViewDelegate(headerViewController),
  m_boxBannerView(BoxBannerView()),
  m_view(BoxView(store, &m_boxBannerView)),
  m_store(store),
  m_displayModeVersion(Expression::DisplayMode::Auto)
{
}

const char * BoxController::title() const {
  return "Boite";
}

View * BoxController::view() {
  AppsContainer * myContainer = ((App *)app())->container();
  Expression::DisplayMode displayMode = myContainer->preferences()->displayMode();
  if (displayMode != m_displayModeVersion) {
    reloadBannerView();
    m_view.reload();
    m_displayModeVersion = displayMode;
  }
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
  reloadBannerView();
  m_view.reload();
}

bool BoxController::isEmpty() const {
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
  m_boxBannerView.setLegendAtIndex((char *)calculationName[(int)m_view.selectedQuantile()], 0);
  char buffer[Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits)];
  CalculPointer calculationMethods[5] = {&Store::minValue, &Store::firstQuartile, &Store::median, &Store::thirdQuartile,
    &Store::maxValue};
  float calculation = (m_store->*calculationMethods[(int)m_view.selectedQuantile()])();
  AppsContainer * container = ((App *)app())->container();
  Complex::convertFloatToText(calculation, buffer, Complex::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits), Constant::LargeNumberOfSignificantDigits, container->preferences()->displayMode());
  m_boxBannerView.setLegendAtIndex(buffer, 1);
}

}
