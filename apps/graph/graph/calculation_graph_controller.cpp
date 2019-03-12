#include "calculation_graph_controller.h"
#include "../app.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

CalculationGraphController::CalculationGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor, I18n::Message defaultMessage) :
  SimpleInteractiveCurveViewController(parentResponder, cursor),
  m_graphView(graphView),
  m_bannerView(bannerView),
  m_graphRange(curveViewRange),
  m_record(),
  m_defaultBannerView(KDFont::SmallFont, defaultMessage, 0.5f, 0.5f, KDColorBlack, Palette::GreyMiddle),
  m_isActive(false)
{
}

void CalculationGraphController::viewWillAppear() {
  assert(!m_record.isNull());
  Expression::Coordinate2D pointOfInterest = computeNewPointOfInteresetFromAbscissa(m_graphRange->xMin(), 1);
  if (std::isnan(pointOfInterest.abscissa)) {
    m_isActive = false;
    m_graphView->setCursorView(nullptr);
    m_graphView->setBannerView(&m_defaultBannerView);
  } else {
    m_isActive = true;
    m_cursor->moveTo(pointOfInterest.abscissa, pointOfInterest.value);
    m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
    reloadBannerView();
  }
  m_graphView->setOkView(nullptr);
  m_graphView->reload();
}

void CalculationGraphController::setRecord(Ion::Storage::Record record) {
  m_graphView->selectRecord(record);
  m_record = record;
}

void CalculationGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(2);
  reloadBannerViewForCursorOnFunction(m_cursor, m_record, functionStore(), CartesianFunction::Symbol());
}

Expression::Coordinate2D CalculationGraphController::computeNewPointOfInteresetFromAbscissa(double start, int direction) {
  App * myApp = static_cast<App *>(app());
  double step = m_graphRange->xGridUnit()/10.0;
  step = direction < 0 ? -step : step;
  double max = direction > 0 ? m_graphRange->xMax() : m_graphRange->xMin();
  return computeNewPointOfInterest(start, step, max, myApp->localContext());
}

CartesianFunctionStore * CalculationGraphController::functionStore() const {
  App * a = static_cast<App *>(app());
  return a->functionStore();
}

bool CalculationGraphController::handleLeftRightEvent(Ion::Events::Event event) {
  if (!m_isActive) {
    return false;
  }
  return SimpleInteractiveCurveViewController::handleLeftRightEvent(event);
}

bool CalculationGraphController::handleEnter() {
  StackViewController * stack = static_cast<StackViewController *>(parentResponder());
  stack->pop();
  return true;
}

bool CalculationGraphController::moveCursorHorizontally(int direction) {
  Expression::Coordinate2D newPointOfInterest = computeNewPointOfInteresetFromAbscissa(m_cursor->x(), direction);
  if (std::isnan(newPointOfInterest.abscissa)) {
    return false;
  }
  m_cursor->moveTo(newPointOfInterest.abscissa, newPointOfInterest.value);
  m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), k_cursorTopMarginRatio, k_cursorRightMarginRatio, k_cursorBottomMarginRatio, k_cursorLeftMarginRatio);
  return true;
}

}
