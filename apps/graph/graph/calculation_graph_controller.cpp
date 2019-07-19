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
  m_defaultBannerView(BannerView::Font(), defaultMessage, 0.5f, 0.5f, BannerView::TextColor(), BannerView::BackgroundColor()),
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
    m_graphRange->panToMakePointVisible(m_cursor->x(), m_cursor->y(), cursorTopMarginRatio(), k_cursorRightMarginRatio, cursorBottomMarginRatio(), k_cursorLeftMarginRatio);
    m_bannerView->setNumberOfSubviews(Shared::XYBannerView::k_numberOfSubviews);
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
  reloadBannerViewForCursorOnFunction(m_cursor, m_record, functionStore(), CartesianFunction::Symbol());
}

Expression::Coordinate2D CalculationGraphController::computeNewPointOfInteresetFromAbscissa(double start, int direction) {
  double step = m_graphRange->xGridUnit()/10.0;
  step = direction < 0 ? -step : step;
  double max = direction > 0 ? m_graphRange->xMax() : m_graphRange->xMin();
  return computeNewPointOfInterest(start, step, max, textFieldDelegateApp()->localContext());
}

CartesianFunctionStore * CalculationGraphController::functionStore() const {
  return App::app()->functionStore();
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
  return true;
}

}
